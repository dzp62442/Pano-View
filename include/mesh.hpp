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
#include "shader.hpp"

struct Vertex {
    glm::vec3 Position; // 顶点位置
    glm::vec3 Normal;   // 顶点法线
    glm::vec2 TexCoords; // 纹理坐标
};

class Mesh {
public:
    // 构造器：初始化网格数据
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    // 渲染网格
    void Draw(Shader &shader);

private:
    // 网格数据
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    // 设置网格的VAO, VBO, EBO等
    void setupMesh();
};
