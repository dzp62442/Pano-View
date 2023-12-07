#include <SVApp.hpp>

#include <csignal>
#include <chrono>
#include <unistd.h>
#include <stdlib.h>
#include <omp.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

bool finish = false;

static void addCar(std::shared_ptr<SVRender> &view_)
{
    glm::mat4 transform_car(1.f);
#ifdef HEMISPHERE
    transform_car = glm::translate(transform_car, glm::vec3(0.f, 0.09f, 0.f));
#else
    // 此处y轴指天，将车模型放在碗模型的地面中央
    transform_car = glm::translate(transform_car, glm::vec3(0.f, 1.01f, 0.f));
#endif
    // 控制车身旋转，采用旋转角+旋转轴表示，此处z轴指天
    // transform_car = glm::rotate(transform_car, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));  // 先绕x轴旋转-90度使车身位于水平面上
    // transform_car = glm::rotate(transform_car, glm::radians(-90.f), glm::vec3(0.f, 0.f, 1.f));  // 再调节绕z轴旋转角度来控制车身朝向
    transform_car = glm::rotate(transform_car, glm::radians(-80.f), glm::vec3(0.f, 1.f, 0.f));  // 调节绕y轴旋转角度来控制车身朝向
    // 控制车身缩放，默认0.002
    transform_car = glm::scale(transform_car, glm::vec3(0.1f));

    bool is_Add = view_->addModel(SVConfig::get().car_model, SVConfig::get().car_vert_shader,
                                  SVConfig::get().car_frag_shader, transform_car);
    if (!is_Add)
        std::cerr << "Error can't add model\n";
}

// SVApp 的构造函数，先构造 SVConfig 的对象，再构造自身的对象
SVApp::SVApp(): cameraSize(SVConfig::get().cam_width, SVConfig::get().cam_height), dataSize(SVConfig::get().data_width, SVConfig::get().data_height),
                GLSize(SVConfig::get().gl_width, SVConfig::get().gl_height), threadpool(SVConfig::get().num_pool_threads)
{
    
}

// 展示拼接好的全景图，2D 或 3D
bool SVApp::showPanorama()
{
    // 系统初始化
    renderer = std::make_shared<SVRender>(GLSize.width, GLSize.height);
    dp = std::make_shared<SVDisplayView>();
    dp->init(GLSize.width, GLSize.height, renderer);
    renderer->init(SVConfig::get().cbowl, SVConfig::get().surroundshadervert, SVConfig::get().surroundshaderfrag,
                    SVConfig::get().screenshadervert, SVConfig::get().screenshaderfrag,
                    SVConfig::get().blackrectshadervert, SVConfig::get().blackrectshaderfrag);
    addCar(renderer);


    cv::Mat panorama;
    int imgNum = SVConfig::get().video_start;   
    std::string panorama_path = SVConfig::get().video_dir;

    if(panorama_path.back() == '/'){  // 存放全景图序列的文件夹
        panorama = cv::imread(panorama_path + std::to_string(imgNum) + ".jpg");
    }
    cv::Size panoramaSize = cv::Size(panorama.cols, panorama.rows);  // 全景图尺寸需保持一致

    while(!finish)  // 开始浏览
    {
        SVTimer show_timer;
        if(panorama_path.back() == '/')  // 存放全景图序列的文件夹
            panorama = cv::imread(panorama_path + std::to_string(imgNum) + ".jpg");
        if (panorama.rows == 0 || panorama.cols == 0){
            LOG_DEBUG("SVApp::showPanorama", "WARNING! Read empty image");
            imgNum++;
            continue;
        }
        else if (panorama.cols != panoramaSize.width || panorama.rows != panoramaSize.height)
            cv::resize(panorama, panorama, panoramaSize);
        show_timer.update("read panorama");

        cv::cuda::GpuMat panorama_gpu;
        panorama_gpu.upload(panorama);
        renderer->setWhiteLuminance(1.0);
        renderer->setToneLuminance(1.0);
        bool okRender = dp->render(panorama_gpu);
        if (!okRender)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 毫秒

        show_timer.update("show");

        std::cout << imgNum << "\n";
        imgNum ++;
        if (imgNum > SVConfig::get().video_stop)
            break;
        show_timer.print("SVApp::showPanorama");
    }

    return true;
}
