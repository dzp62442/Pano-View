#pragma once
#include <chrono>
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

struct ConfigBowl
{
    float a, b, c;
    float disk_radius;
    float parab_radius;
    float hole_radius;
    float vertices_num;
    float y_start = 0.0;
    glm::mat4 transformation;
    ConfigBowl() : a(0.0f), b(0.0f), c(0.0f), disk_radius(0.0f), parab_radius(0.0f), hole_radius(0.0f), vertices_num(0.0f) {}
    ConfigBowl(const float a_, const float b_, const float c_,
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
    // 读取相机时：cameraSize 为启动相机的尺寸，dataSize 为用于拼接的尺寸，通常需要程序中进行 resize
    // 读取视频时：cameraSize 和 dataSize 均为视频图片的尺寸，程序中不需要进行 resize
    int cam_width = 1280;
    int cam_height = 720;
    int data_width = 1280;
    int data_height = 720;
    int gl_width = data_width;
    int gl_height = data_height;
    int num_pool_threads = 8;  // threadpool线程数
    float scale_factor = 1;
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

    ConfigBowl cbowl;  // 碗模型参数

private:
    SVConfig()  // 私有构造函数，确保不能直接创建对象
    {
        YAML::Node yaml = YAML::LoadFile("../configs/svcfg.yaml");


        video_start = yaml["video_start"].as<int>();
        video_stop = yaml["video_stop"].as<int>();

        video_dir = yaml["video_dir"].as<std::string>();

        // 加载碗模型参数
        glm::mat4 transform_bowl(1.f);
        cbowl.transformation = transform_bowl;
        cbowl.disk_radius = yaml["cbowl"]["disk_radius"].as<float>();
        cbowl.parab_radius = yaml["cbowl"]["parab_radius"].as<float>(); 
        cbowl.hole_radius = yaml["cbowl"]["hole_radius"].as<float>();
        cbowl.a = yaml["cbowl"]["a"].as<float>();
        cbowl.b = yaml["cbowl"]["b"].as<float>();
        cbowl.c = yaml["cbowl"]["c"].as<float>();
        cbowl.vertices_num = yaml["cbowl"]["vertices_num"].as<float>();
        cbowl.y_start = yaml["cbowl"]["y_start"].as<float>();

    }

    ~SVConfig() = default;
    SVConfig(const SVConfig&) = delete;  // 禁用拷贝构造函数
    SVConfig& operator=(const SVConfig&) = delete;  // 禁用赋值运算符
};