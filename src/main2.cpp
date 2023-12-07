#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <thread>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <exception>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <string>

//! 全局变量
GLfloat  xangle = 0.0;    //X 旋转量
GLfloat  yangle = 0.0;    //Y 旋转量
GLfloat  zangle = 0.0;    //Z 旋转量
float vertex[61][31][3];
float texpoint[61][31][2];
cv::VideoCapture cap;
int Width = 0;
int Height = 0;
static GLuint texName[4];//存放纹理
static GLint Model = 0; // 模型 calllist 号
static double Ox = 0;   // 视角旋转参数
static double Oy = 0;
static double direction = 0; //站立位置及前进方向；
static double step = 0.5;
GLUquadricObj*g_text;//曲面，制作一个包围房间的大球体做背景
//交叉点的坐标
int cx = 0;
int cy = 0;
// 全局变量，记录鼠标按下状态和位置
bool isDragging = false;
double lastX = 0, lastY = 0;

//初始化
void init(void)
{
    glClearColor (0.0, 0.0, 0.0, 0.0);    //清理颜色，为黑色，（也可认为是背景颜色）

    glCullFace(GL_FRONT);    //背面裁剪(背面不可见)
    glEnable(GL_CULL_FACE);    //启用裁剪
    glEnable(GL_TEXTURE_2D);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//所读取的图像数据的行对齐方式

    //初始化数据
    for(int i=0; i<=360; i+=6)
    {
        for( int j=180; j>=0; j-=6)
        {
            vertex[i/6][(180-j)/6][0] = cos((float)i/180.0*CV_PI)*sin((float)j/180.0*CV_PI)*2;
            vertex[i/6][(180-j)/6][1] = cos((float)j/180.0*CV_PI)*2;
            vertex[i/6][(180-j)/6][2] = sin((float)i/180.0*CV_PI)*sin((float)j/180.0*CV_PI)*2;
        }
    }
    for(int i=0; i<61; i++)
    {
        for(int k=0; k<31; k++)
        {
            texpoint[i][k][0]= (float)i/60.0;        //生成X浮点值
            texpoint[i][k][1]= (float)k/30.0;        //生成Y浮点值
        }
    }
    printf("...\n");
    glGenTextures(1, &texName[0]);//用来生成纹理的数量  存储纹理索引的第一个元素指针
    glBindTexture(GL_TEXTURE_2D, texName[0]);//绑定（指定）纹理对象
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);   //纹理参数：坐标超过0.0~1.0f时的处理方式--重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  //纹理参数：贴图插值方式--最近邻 / 线性插值 GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void loadTexture(const std::string &filename, GLuint &texture)
{
    cv::Mat frame;
    frame = cv::imread(filename);
    int ImageWidth = frame.cols;
    int ImageHeight = frame.rows;
    cv::flip(frame, frame, 0);

    //加载纹理
    glGenTextures(1, &texture);  //用来生成纹理的数量  存储纹理索引的第一个元素指针
    glBindTexture(GL_TEXTURE_2D, texture);  //绑定（指定）纹理对象
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);   //纹理参数：坐标超过0.0~1.0f时的处理方式--重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  //纹理参数：贴图插值方式--最近邻 / 线性插值 GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,  ImageWidth, ImageHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, frame.data);

    //生成一个2D纹理（Texture）。bmp图片的像素顺序是BGR所以用GL_BGR_EXT来反向加载数据
    std::cout << texture << "\n";
}

//从这里开始进行所有的绘制
void drawCube(void)
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    //清除屏幕和深度缓存
    glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();    //重置当前的模型观察矩阵

    glPushMatrix();
    {
        gluLookAt(0, 0, -5, 0, 0, 0, 0, 1, 0);
        glTranslatef(0.0f, 0.0f, -5.0f);    //移入屏幕 5 个单位
        glRotatef(xangle, 1.0f, 0.0f, 0.0f);    //绕X轴旋转
        glRotatef(yangle, 0.0f, 1.0f, 0.0f);    //绕Y轴旋转
        glRotatef(zangle, 0.0f, 0.0f, 1.0f);    //绕Z轴旋转
        
        glBindTexture(GL_TEXTURE_2D, texName[0]);                // 选择纹理,有多个纹理时这句话是必要de
        glBegin(GL_QUADS); {    //四边形绘制开始
            for(int i=0; i<60; i++) {
                for(int j=0; j<30; j++) {
                    //第一个纹理坐标(左下角)
                    glTexCoord2f(texpoint[i][j][0], texpoint[i][j][1]);
                    glVertex3f(vertex[i][j][0], vertex[i][j][1], vertex[i][j][2]);

                    //第二个纹理坐标(左上角)
                    glTexCoord2f(texpoint[i][j][0], texpoint[i][j+1][1]);
                    glVertex3f(vertex[i][j+1][0], vertex[i][j+1][1], vertex[i][j+1][2]);

                    //第三个纹理坐标(右上角)
                    glTexCoord2f(texpoint[i+1][j+1][0], texpoint[i+1][j+1][1]);
                    glVertex3f(vertex[i+1][j+1][0], vertex[i+1][j+1][1], vertex[i+1][j+1][2]);

                    //第四个纹理坐标(右下角)
                    glTexCoord2f(texpoint[i+1][j][0], texpoint[i][j][1]);
                    glVertex3f(vertex[i+1][j][0], vertex[i+1][j][1], vertex[i+1][j][2]);
                }
            }
        }glEnd();

    }
    glPopMatrix();
    glFlush();
}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    //清除颜色数据和深度数据（清屏）
    //glLoadIdentity();    //Reset The View
    glPushMatrix(); // 保存当前模型视图矩阵
    glRotatef((GLfloat)direction, 0.0, 0.0, 1.0);   // 用变换后(旋转或移动)模型坐标系建立模型
    glTranslated(Ox, Oy, 0.0);

    // looploadTexture(texName[0]);
    loadTexture("../resources/total.bmp", texName[0]);
    drawCube();
    glPopMatrix();
}

//当窗口大小改变时，会调用这个函数
void reshape(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);  //设置视口
    glMatrixMode(GL_PROJECTION);  //设置矩阵模式为投影变换矩阵，
    glLoadIdentity();  //变为单位矩阵
    gluPerspective(110, (GLfloat)width / height, 0.1f, 100.0f);  //设置投影矩阵
    glMatrixMode(GL_MODELVIEW);  //设置矩阵模式为视图矩阵(模型)
    glLoadIdentity();  //变为单位矩阵
}

//处理鼠标点击
void Mouse(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // 当鼠标左键按下，记录位置并标记拖拽状态
            isDragging = true;
            glfwGetCursorPos(window, &lastX, &lastY);
            std::cout << lastX << lastY << "\n";
        } else if (action == GLFW_RELEASE) {
            // 当鼠标左键释放，取消拖拽状态
            isDragging = false;
        }
    }
}

//处理鼠标拖动
void onMouseMove(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging) {
        // 计算鼠标移动的偏移量
        double xoffset = xpos - lastX;
        double yoffset = lastY - ypos; // 注意：y坐标是从下到上增加的

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        // 更新视角旋转
        yangle -= xoffset;
        if (xangle < 90 && yangle > -90) {
            xangle += yoffset;
        } else if (xangle > -90 && yangle < 90) {
            xangle += yoffset;
        }

        // 更新最后的鼠标位置
        lastX = xpos;
        lastY = ypos;
    }
}

//键盘输入事件函数
void keyboard(unsigned char key,int x,int y)
{
    switch(key)
    {
        case 'x':        //当按下键盘上d时，以沿X轴旋转为主
            if (xangle < 85.0f)
            {
                xangle += 1.0f;    //设置旋转增量
            }
            break;
        case 'X':
            if (xangle > -85.0f)
            {
                xangle -= 1.0f;    //设置旋转增量
            }
            break;
        case 'y':
            yangle += 1.0f;
            break;
        case 'Y':
            yangle -= 1.0f;
            break;
        case 'w':
        case 'W':
            Ox -= step*sin(direction / 180 * CV_PI);
            Oy -= step*cos(direction / 180 * CV_PI);
            glutPostRedisplay();
            break;
        case 's':
        case 'S':
            Ox += step*sin(direction / 180 *CV_PI);
            Oy += step*cos(direction / 180 *CV_PI);
            glutPostRedisplay();
            break;
        case 'a':
        case 'A':
            direction = direction - 2;
            glutPostRedisplay();
            break;
        case 'd':
        case 'D':
            direction = direction + 2;
            glutPostRedisplay();
            break;
        default:
            return;
    }
    glutPostRedisplay();    //重绘函数
}

//特殊按键
void specialKey(int key, int x, int y)
{
    float offset = 1.5;
    switch (key)
    {
    case GLUT_KEY_UP:    //脑袋向上往前看
        if (xangle < 90.0f)
        {
            xangle += offset;    //设置旋转增量
        }
        break;
    case GLUT_KEY_DOWN:    //脑袋向下往前看
        if (xangle > -90.0f)
        {
            xangle -= offset;    //设置旋转增量
        }
        break;
    case GLUT_KEY_LEFT:    //脑袋想左往前看
        yangle -= offset;
        break;
    case GLUT_KEY_RIGHT:    //脑袋向右往前看
        yangle += offset;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    // 初始化GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Panoramic View", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 设置OpenGL上下文
    glfwMakeContextCurrent(window);

    // 初始化OpenGL状态和数据
    init(); 

    // GLFW窗口大小变化的回调函数
    glfwSetFramebufferSizeCallback(window, reshape);

    // GLFW键盘事件的回调函数
    // glfwSetKeyCallback(window, keyboard);

    // GLFW鼠标事件的回调函数
    glfwSetMouseButtonCallback(window, Mouse);
    glfwSetCursorPosCallback(window, onMouseMove);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 渲染操作
        display();

        // 交换缓冲
        glfwSwapBuffers(window);

        // 处理事件
        glfwPollEvents();
    }

    // 终止GLFW
    glfwTerminate();
    return 0;
}