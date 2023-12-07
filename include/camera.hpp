#pragma once

#include <cmath>
#include <GLFW/glfw3.h>
#include <GL/glew.h> // 使用GLEW管理OpenGL函数
#include <GL/gl.h>   // 核心OpenGL功能
#include <glm/glm.hpp> // 用于数学运算，例如向量和矩阵
#include <glm/gtc/matrix_transform.hpp> // 提供矩阵变换功能
#include <string>
#include <vector>


class Camera {
public:
    // 默认构造器
    Camera() 
        : Position(glm::vec3(0.0f, 0.0f, 0.0f)), 
          Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
          Up(glm::vec3(0.0f, 1.0f, 0.0f)),
          Right(glm::vec3(1.0f, 0.0f, 0.0f)),
          WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
          Yaw(-90.0f), 
          Pitch(0.0f), 
          MovementSpeed(2.5f), 
          MouseSensitivity(0.1f), 
          Zoom(45.0f) 
    {
        updateCameraVectors();
    }

    // 构造函数
    Camera(glm::vec3 position);

    // 获取观察矩阵
    glm::mat4 GetViewMatrix();

    // 处理输入控制相机移动
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // 处理输入控制相机视角
    void ProcessMouseMovement(float xoffset, float yoffset);

    // 设置投影参数
    void setPerspective(float fov, float aspect, float near, float far);

    // 设置相机位置
    void setPosition(float x, float y, float z);

private:
    // 相机属性
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // 欧拉角
    float Yaw;
    float Pitch;

    // 相机选项
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // 更新相机的向量
    void updateCameraVectors();
};
