#pragma once
#include <chrono>
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

struct ConfigProjModel
{
    float a, b, c;
    float disk_radius;
    float parab_radius;
    float hole_radius;
    float vertices_num;
    float y_start = 0.0;
    glm::mat4 transformation;
    ConfigProjModel() : a(0.0f), b(0.0f), c(0.0f), disk_radius(0.0f), parab_radius(0.0f), hole_radius(0.0f), vertices_num(0.0f) {}
    ConfigProjModel(const float a_, const float b_, const float c_,
               const float disk_radius_, const float parab_radius_, const float hole_radius_,
               const float vertices_num_) :
                a(a_), b(b_), c(c_), disk_radius(disk_radius_), parab_radius(parab_radius_),
                hole_radius(hole_radius_), vertices_num(vertices_num_)
    {}
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
    std::string win1 = "Cam0"; // window name
    
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

    ConfigProjModel cfg_proj;  // 碗模型参数

private:
    SVConfig()  // 私有构造函数，确保不能直接创建对象
    {
        YAML::Node yaml = YAML::LoadFile("../configs/svcfg.yaml");


        video_start = yaml["video_start"].as<int>();
        video_stop = yaml["video_stop"].as<int>();

        video_dir = yaml["video_dir"].as<std::string>();

        // 加载碗模型参数
        glm::mat4 transform_bowl(1.f);
        cfg_proj.transformation = transform_bowl;
        cfg_proj.disk_radius = yaml["cfg_proj"]["disk_radius"].as<float>();
        cfg_proj.parab_radius = yaml["cfg_proj"]["parab_radius"].as<float>(); 
        cfg_proj.hole_radius = yaml["cfg_proj"]["hole_radius"].as<float>();
        cfg_proj.a = yaml["cfg_proj"]["a"].as<float>();
        cfg_proj.b = yaml["cfg_proj"]["b"].as<float>();
        cfg_proj.c = yaml["cfg_proj"]["c"].as<float>();
        cfg_proj.vertices_num = yaml["cfg_proj"]["vertices_num"].as<float>();
        cfg_proj.y_start = yaml["cfg_proj"]["y_start"].as<float>();

    }

    ~SVConfig() = default;
    SVConfig(const SVConfig&) = delete;  // 禁用拷贝构造函数
    SVConfig& operator=(const SVConfig&) = delete;  // 禁用赋值运算符
};