#pragma once

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <GLFW/glfw3.h>
#include <GL/glew.h> // 使用GLEW管理OpenGL函数
#include <GL/gl.h>   // 核心OpenGL功能
#include <glm/glm.hpp> // 用于数学运算，例如向量和矩阵
#include <glm/gtc/matrix_transform.hpp> // 提供矩阵变换功能
#include <string>
#include <vector>

class Shader {
public:
    // 程序ID
    unsigned int ID;

    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath);

    // 使用/激活程序
    void use();

    // uniform工具函数
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    // 检查着色器编译/链接错误
    void checkCompileErrors(unsigned int shader, std::string type);
};
