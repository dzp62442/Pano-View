#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <thread>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <exception>
#include <GLFW/glfw3.h>
#include <GL/glew.h> // 使用GLEW管理OpenGL函数
#include <GL/gl.h>   // 核心OpenGL功能
#include <glm/glm.hpp> // 用于数学运算，例如向量和矩阵
#include <glm/gtc/matrix_transform.hpp> // 提供矩阵变换功能
#include <string>
#include "mesh.hpp"
#include "camera.hpp"

int main(int argc, char *argv[])
{
    // 初始化GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Panorama Viewer", NULL, NULL);
    glfwMakeContextCurrent(window);

    // 初始化OpenGL状态
    // 设置视口，配置投影等

    // 加载全景图像
    GLuint panoramaTexture = loadPanoramaTexture("path/to/panorama.jpg");

    // 创建球面网格
    Mesh sphereMesh = createSphereMesh();

    // 设置虚拟相机
    Camera camera;
    camera.setPosition(0, 0, 0); // 球面中心
    camera.setPerspective(fov, aspect, near, far);

    // 渲染循环
    while(!glfwWindowShouldClose(window)) {
        // 处理用户输入
        processInput(window);

        // 更新相机视角等

        // 渲染
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderSphere(panoramaTexture, sphereMesh, camera);

        // 交换缓冲区并轮询IO事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理资源
    glfwDestroyWindow(window);
    glfwTerminate();


    return 0;
}