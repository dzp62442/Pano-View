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
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include "glm/glm.hpp"
#include <string>
#define  pi 3.141592654
#define PI 3.14159265359

//! 全局变量
time_t start;
time_t eend;
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

//初始化
void init(void)
{
    glClearColor (0.0, 0.0, 0.0, 0.0);    //清理颜色，为黑色，（也可认为是背景颜色）

    glCullFace(GL_FRONT);    //背面裁剪(背面不可见)
    glEnable(GL_CULL_FACE);    //启用裁剪
    glEnable(GL_TEXTURE_2D);
    //loadGLTexture2();    //载入纹理贴图
    //loadTexture("../20221101_2D_0.mp4",texName[0]);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//所读取的图像数据的行对齐方式

    //初始化数据
    for(int i=0; i<=360; i+=6)
    {
        for( int j=180; j>=0; j-=6)
        {
            vertex[i/6][(180-j)/6][0] = cos((float)i/180.0*pi)*sin((float)j/180.0*pi)*2;
            vertex[i/6][(180-j)/6][1] = cos((float)j/180.0*pi)*2;
            vertex[i/6][(180-j)/6][2] = sin((float)i/180.0*pi)*sin((float)j/180.0*pi)*2;
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

void loadTexture(const char* filename,GLuint &texture)
{
    cv::Mat frame;
    // frame = imread("../panorama_1227_5.jpg");
    frame = cv::imread("../resources/total.bmp");
    int ImageWidth = frame.cols;
    int ImageHeight = frame.rows;
    flip(frame, frame, 0);

    //加载纹理
    glGenTextures(1, &texture);//用来生成纹理的数量  存储纹理索引的第一个元素指针
    glBindTexture(GL_TEXTURE_2D, texture);//绑定（指定）纹理对象
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);   //纹理参数：坐标超过0.0~1.0f时的处理方式--重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  //纹理参数：贴图插值方式--最近邻 / 线性插值 GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, weight, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, frame.data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,  ImageWidth, ImageHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, frame.data);

    //生成一个2D纹理（Texture）。bmp图片的像素顺序是BGR所以用GL_BGR_EXT来反向加载数据
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    //清楚颜色数据和深度数据（清屏）
    //glLoadIdentity();    //Reset The View
    glPushMatrix(); // 保存当前模型视图矩阵
    glRotatef((GLfloat)direction, 0.0, 0.0, 1.0);   // 用变换后(旋转或移动)模型坐标系建立模型
    glTranslated(Ox, Oy, 0.0);

    // looploadTexture(texName[0]);
    loadTexture("../resources/total.bmp", texName[0]);
    drawCube();

    glutSwapBuffers();    //交换缓冲区。显示图形
    
}

//当窗口大小改变时，会调用这个函数
void reshape(GLsizei w,GLsizei h)
{
    glViewport(0,0,w,h);    //设置视口
    glMatrixMode(GL_PROJECTION);    //设置矩阵模式为投影变换矩阵，
    glLoadIdentity();    //变为单位矩阵
    gluPerspective(110, (GLfloat)w / h, 0.1f, 100.0f);    //设置投影矩阵
    glMatrixMode(GL_MODELVIEW);    //设置矩阵模式为视图矩阵(模型)
    glLoadIdentity();    //变为单位矩阵
}

//处理鼠标点击
void Mouse(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN) //第一次鼠标按下时,记录鼠标在窗口中的初始坐标
    {
        //记住鼠标点击后光标坐标
        cx = x;
        cy = y;
        //printf("Mouse: x=%d, y=%d, oldx_Translatef=%f, oldy_Translatef=%f\n", x, y, oldx_Translatef, oldy_Translatef);
    }
}

//处理鼠标拖动
void onMouseMove(int x, int y)
{
    float offset = 0.18;
    //计算拖动后的偏移量，然后进行xy叠加减
    yangle -= ((x - cx) * offset);

    if (xangle < 90 && y > cy) {//往下拉
        xangle += ((y - cy) * offset);
    } else if (xangle > -90 && y < cy) {//往上拉
        xangle += ((y - cy) * offset);
    }
    //printf("Move: x=%d(%d)[%d], y=%d(%d)[%d], xangle_Textures=%f, yangle_Textures=%f\n",
    //    x, cx_Textures, x-cx_Textures,
    //    y, cy_Textures, y-cy_Textures,
    //    xangle_Textures, yangle_Textures);
    glutPostRedisplay();

    //保存好当前拖放后光标坐标点
    cx = x;
    cy = y;
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
            Ox -= step*sin(direction / 180 * PI);
            Oy -= step*cos(direction / 180 * PI);
            glutPostRedisplay();
            break;
        case 's':
        case 'S':
            Ox += step*sin(direction/180*PI);
            Oy += step*cos(direction/180*PI);
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
        //case 'z':
        //    zangle += 1.0f;
        //    break;
        //case 'Z':
        //    zangle -= 1.0f;
        //    break;
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
    
    cap.open("../resources/20221105_2D_0.mp4");
    if (!cap.isOpened() ){
        printf("Cannot open the video");
        exit(0);
    }
    Width = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
    Height = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    //printf("可通过↑↓←→按键控制全景图绕旋转\n");
    glutInit(&argc, argv);    //固定格式
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(800, 800);    //显示框的大小
    glutInitWindowPosition(100,100);    //确定显示框左上角的位置
    glutCreateWindow("OpenGLsurroundview");

    init();    //初始化资源,这里一定要在创建窗口以后，不然会无效

    glutDisplayFunc(display);
    // glutTimerFunc(40, timeFunc, 0);

    glutReshapeFunc(reshape); 
    glutMouseFunc(Mouse);
    glutMotionFunc(onMouseMove);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey); // 特殊按键

    glutMainLoop();

    cap.release();

    return 0;
}