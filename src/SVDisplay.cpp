#include <SVDisplay.hpp>


extern bool finish; // from SVApp

float lastX = 1280 / 2.f; // last x pos cursor
float lastY = 720 / 2.f; // last y pos cursor
float deltaTime = 0.f;
float lastFrame = 0.f;
bool firstMouse = true;
bool demo_key_press = false;
bool topview_key_press = false;


static void frame_buffer_size_callback(GLFWwindow* wnd, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void processMouse(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed: y ranges bottom to top
    lastX = xpos;
    lastY = ypos;

    cam.processMouseMovement(xoffset, yoffset);
}

static void processScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.processMouseScroll(yoffset);
}

/*
V: 旋转观察
T: 俯视
R: 重置相机
Q: 退出
*/
static void processInput(GLFWwindow* window, SVDisplayView* svdisp)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    const float cameraSpeed = 1.0f * deltaTime; //2.5f
    constexpr auto const_speed = 0.1f;

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && !demo_key_press) {
        svdisp->setTVMode(false);
        svdisp->setSVMode(!svdisp->getSVMode());
        demo_key_press = true;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE) {
        demo_key_press = false;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !topview_key_press) {
        svdisp->setSVMode(false);
        svdisp->setTVMode(!svdisp->getTVMode());
        topview_key_press = true;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
        topview_key_press = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        finish = true;
    }

    /* reset camera position */
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        svdisp->setSVMode(false);
        svdisp->setTVMode(false);
        svdisp->resetCameraState();
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam.processKeyboard(Camera_Movement::FORWARD, const_speed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cam.processKeyboard(Camera_Movement::BACKWARD, const_speed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cam.processKeyboard(Camera_Movement::LEFT, const_speed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cam.processKeyboard(Camera_Movement::RIGHT, const_speed);
    }

    // 按空格保存渲染图像
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        int width = svdisp->getWindowWidth();
        int height = svdisp->getWindowHeight();
        // 读取数据
        GLubyte* pixels = new GLubyte[width * height * 3]; // 3 代表 RGB
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        cv::Mat image(height, width, CV_8UC3, pixels);
        // 处理图像
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        cv::flip(image, image, 0); // 水平翻转，0代表沿x轴翻转
        // 保存图像
        std::string save_dir = SVConfig::get().video_dir + "3d-render/";
        if (access(save_dir.c_str(), 0)){  // 判断文件夹是否存在，不存在则创建
            LOG_DEBUG("SVDisplay::processInput", "Folder to save 3d-render images not exist! mkdir.");
            std::string command = "mkdir -p " + save_dir;
            system(command.c_str());
        }
        std::time_t now = std::time(nullptr);  // 获取当前时间
        std::tm* now_tm = std::localtime(&now);  // 将time_t转换为tm结构体
        std::stringstream ss;  // 创建一个字符串流
        ss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");  // 使用put_time将tm结构体格式化为字符串
        std::string time_str = ss.str();  // 从字符串流中获取字符串
        cv::imwrite(save_dir+time_str+".png", image);
        LOG_DEBUG("SVDisplay::processInput", "Save 3d-render image to %s.", save_dir.c_str());
        delete[] pixels;
    }

}

bool SVDisplayView::init(const int32 wnd_width, const int32 wnd_height, std::shared_ptr<SVRender3D> renderer_)
{
    if (isInit)
        return isInit;

    this->width = wnd_width;
    this->height = wnd_height;
    aspect_ratio = static_cast<float>(wnd_width) / wnd_height;
    renderer = renderer_;
    
    // 初始化 glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 大版本号，可以设为4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);  // 小版本号，可以设为6
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    // 创建窗口
    window = glfwCreateWindow(width, height, "Surround View", NULL, NULL);
    if (window == nullptr) {
        LOG_ERROR("SVDisplayView::init", "Failed create GLFW window !");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);  // 设置当前线程的 OpenGL 上下文，上下文与窗口关联

    glEnable(GL_DEPTH_TEST);  // 启用深度测试，用于处理图元遮挡关系
    //glDepthFunc(GL_LEQUAL);  // 设置深度测试的条件为 “小于或等于”

    glViewport(0, 0, width, height);  // 设置视口

    // 窗口输入与回调函数
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // 输入模式：不显示光标且光标不会离开窗口
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);  // 帧缓冲大小回调函数，处理窗口尺寸变化
    glfwSetCursorPosCallback(window, processMouse);  // 鼠标位置回调函数，处理鼠标移动事件
    glfwSetScrollCallback(window, processScroll);  // 鼠标滚轮回调函数

    isInit = true;
    return isInit;
}

bool SVDisplayView::render(const cv::cuda::GpuMat& frame)
{
    if (!glfwWindowShouldClose(window) && isInit) {
        // input
        processInput(window, this);

        if (useDemoSurroundView)
            demoSVMode(cam);
        if (useDemoTopView)
            demoTVMode(cam);

        if (renderer->getInit()){
            renderer->render(cam, frame);
        }

        // check and call events and swap the buffers
        glfwPollEvents();
        glfwSwapBuffers(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }
    else
        return false;

    return true;
}


void SVDisplayView::resetCameraState()
{
    cam = Camera(glm::vec3(0.0, 1.7, 1.0), glm::vec3(0.0, 1.0, 0.0));
}

void SVDisplayView::demoSVMode(Camera& camera)
{
    constexpr auto const_speed = 0.5;
    abs_yaw_sv_mode += const_speed;
    if (abs_yaw_sv_mode > 360)
        abs_yaw_sv_mode = 0;
    cam.processAbsAxis(abs_yaw_sv_mode);
}

void SVDisplayView::demoTVMode(Camera& camera)
{
    constexpr auto angle = -90.0;
    constexpr auto rot_angle_x = 7.5;
    camera.processMouseMovement(0, angle);
    camera.processMouseMovement(rot_angle_x, 0);
}

void SVDisplayView::setSVMode(const bool demo)
{
    useDemoSurroundView = demo;
    resetCameraState();
    cam.setCamPos(glm::vec3(0, 2.5, 0.0));
    abs_yaw_sv_mode = 0;
}

bool SVDisplayView::getSVMode() const
{
    return useDemoSurroundView;
}

void SVDisplayView::setTVMode(const bool topview)
{
    useDemoTopView = topview;
    resetCameraState();
    cam.setCamPos(glm::vec3(0, 4.15, 0.0));
}

bool SVDisplayView::getTVMode() const
{
    return useDemoTopView;
}
