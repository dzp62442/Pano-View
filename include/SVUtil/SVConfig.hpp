#pragma once
#include <chrono>
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

struct ConfigProjModel
{
    float vertices_num = 750;
    float y_start = 0.0;
    float top_black_height_scale = 0.0, bottom_black_height_scale = 0.0;
    float a = 0.0, b = 0.0, c = 0.0;
    float inner_radius = 0.4;
    float radius = 0.55;
    float hole_radius = 0.08;
    float x_segment = 100, y_segment = 100;
    float max_height = 1, rad_at_base = 4;
    float R_burger = 4, d_burger = 2;
    ConfigProjModel() {}
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

    ConfigProjModel proj_cfg;  // 投影模型参数

private:
    SVConfig()  // 私有构造函数，确保不能直接创建对象
    {
        YAML::Node yaml = YAML::LoadFile("../configs/svcfg.yaml");

        video_start = yaml["video_start"].as<int>();
        video_stop = yaml["video_stop"].as<int>();

        video_dir = yaml["video_dir"].as<std::string>();
        proj_type = yaml["proj_type"].as<std::string>();

        // 加载投影模型参数
        proj_cfg.vertices_num = yaml["proj_cfg"]["vertices_num"].as<float>();
        proj_cfg.y_start = yaml["proj_cfg"]["y_start"].as<float>();
        proj_cfg.top_black_height_scale = yaml["proj_cfg"]["top_black_height_scale"].as<float>();
        proj_cfg.bottom_black_height_scale = yaml["proj_cfg"]["bottom_black_height_scale"].as<float>();
        proj_cfg.inner_radius = yaml["proj_cfg"]["inner_radius"].as<float>();
        proj_cfg.radius = yaml["proj_cfg"]["radius"].as<float>(); 
        proj_cfg.hole_radius = yaml["proj_cfg"]["hole_radius"].as<float>();
        proj_cfg.a = yaml["proj_cfg"]["a"].as<float>();
        proj_cfg.b = yaml["proj_cfg"]["b"].as<float>();
        proj_cfg.c = yaml["proj_cfg"]["c"].as<float>();
        proj_cfg.x_segment = yaml["proj_cfg"]["x_segment"].as<float>();
        proj_cfg.y_segment = yaml["proj_cfg"]["y_segment"].as<float>();
        proj_cfg.max_height = yaml["proj_cfg"]["max_height"].as<float>();
        proj_cfg.rad_at_base = yaml["proj_cfg"]["rad_at_base"].as<float>();
        proj_cfg.R_burger = yaml["proj_cfg"]["R_burger"].as<float>();
        proj_cfg.d_burger = yaml["proj_cfg"]["d_burger"].as<float>();
    }

    ~SVConfig() = default;
    SVConfig(const SVConfig&) = delete;  // 禁用拷贝构造函数
    SVConfig& operator=(const SVConfig&) = delete;  // 禁用赋值运算符
};