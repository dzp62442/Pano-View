#pragma once
#include <chrono>
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

struct ConfigProjModel
{
    float a, b, c;
    float inner_radius;
    float radius;
    float hole_radius;
    float vertices_num;
    float y_start = 0.0;
    float x_segment, y_segment;
    glm::mat4 transformation;
    ConfigProjModel() : a(0.0f), b(0.0f), c(0.0f), inner_radius(0.0f), radius(0.0f), hole_radius(0.0f), vertices_num(0.0f), x_segment(0.0f), y_segment(0.0f) {}

};

// 使用单例模式构建全局参数类
class SVConfig
{
public:
    static SVConfig& get() {
        static SVConfig instance;
        return instance;
    }

    //参数
    int gl_width = 1280;
    int gl_height = 720;
    int num_pool_threads = 8;  // threadpool线程数
    int video_start;
    int video_stop;
    std::string proj_type;
    
    // 文件路径
    std::string video_dir;
    // std::string car_model = "../models/car/Dodge Challenger SRT Hellcat 2015.obj";
    // std::string car_model = "../models/truck/truck.obj";
    std::string car_model = "../models/OBJ/truck_daf.obj";
    std::string car_vert_shader = "../shaders/modelshadervert.glsl";
    std::string car_frag_shader = "../shaders/modelshaderfrag.glsl"; 
    std::string surroundshadervert = "../shaders/surroundvert.glsl";
    std::string surroundshaderfrag = "../shaders/surroundfrag.glsl";
    std::string screenshadervert = "../shaders/frame_screenvert.glsl";
    std::string screenshaderfrag = "../shaders/frame_screenfrag.glsl";
    std::string blackrectshadervert = "../shaders/blackrectshadervert.glsl";
    std::string blackrectshaderfrag = "../shaders/blackrectshaderfrag.glsl";

    ConfigProjModel proj_cfg;  // 碗模型参数

private:
    SVConfig()  // 私有构造函数，确保不能直接创建对象
    {
        YAML::Node yaml = YAML::LoadFile("../configs/svcfg.yaml");


        video_start = yaml["video_start"].as<int>();
        video_stop = yaml["video_stop"].as<int>();

        video_dir = yaml["video_dir"].as<std::string>();
        proj_type = yaml["proj_type"].as<std::string>();

        // 加载碗模型参数
        glm::mat4 transform_bowl(1.f);
        proj_cfg.transformation = transform_bowl;
        proj_cfg.inner_radius = yaml["proj_cfg"]["inner_radius"].as<float>();
        proj_cfg.radius = yaml["proj_cfg"]["radius"].as<float>(); 
        proj_cfg.hole_radius = yaml["proj_cfg"]["hole_radius"].as<float>();
        proj_cfg.a = yaml["proj_cfg"]["a"].as<float>();
        proj_cfg.b = yaml["proj_cfg"]["b"].as<float>();
        proj_cfg.c = yaml["proj_cfg"]["c"].as<float>();
        proj_cfg.vertices_num = yaml["proj_cfg"]["vertices_num"].as<float>();
        proj_cfg.y_start = yaml["proj_cfg"]["y_start"].as<float>();
        proj_cfg.x_segment = yaml["proj_cfg"]["x_segment"].as<float>();
        proj_cfg.y_segment = yaml["proj_cfg"]["y_segment"].as<float>();
    }

    ~SVConfig() = default;
    SVConfig(const SVConfig&) = delete;  // 禁用拷贝构造函数
    SVConfig& operator=(const SVConfig&) = delete;  // 禁用赋值运算符
};