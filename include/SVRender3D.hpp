#pragma once
#include <vector>

#include <ProjModel.hpp>
#include <Virtcam.hpp>
#include <ObjModel.hpp>
#include <SVCudaOGL.hpp>


class SVRender3D
{
private:
    bool initProjModel(const std::string& filesurroundvert, const std::string& filesurroundfrag);
    bool initBlackRect(const std::string& fileblackrectvert, const std::string& fileblackrectfrag);
    bool initQuadRender(const std::string& filescreenvert, const std::string& filescreenfrag);
protected:
    void texturePrepare(const cv::cuda::GpuMat& frame);
    void drawSurroundView(const Camera& cam, const cv::cuda::GpuMat& frame);
    void drawObjModel(const Camera& cam);
    void drawScreen(const Camera& cam);
    void drawBlackRect(const Camera& cam);
public:  
    bool getInit() const{return isInit;}
    bool addObjModel(const std::string& pathmodel, const std::string& pathvertshader, const std::string& pathfragshader);
    float getWhiteLuminance() const{return white_luminance;}
    void setWhiteLuminance(const float white_luminance_) {white_luminance = white_luminance_;}
    float getToneLuminance() const{return tonemap_luminance;}
    void setToneLuminance(const float tone_luminance_) {tonemap_luminance = tone_luminance_;}

public:
    SVRender3D(const int32 wnd_width_, const int32 wnd_height_, const std::string proj_type_) : 
               wnd_width(wnd_width_), wnd_height(wnd_height_), proj_type(proj_type_),
               aspect_ratio(0.f), texReady(false), white_luminance(1.0), tonemap_luminance(1.0)
               {}

    SVRender3D& operator=(const SVRender3D&) = delete;  // 禁用拷贝赋值运算符
    SVRender3D(const SVRender3D&) = delete;  // 禁用拷贝构造函数

    bool init(const ConfigProjModel& proj_cfg_, const std::string& shadersurroundvert, const std::string& shadersurroundfrag,
              const std::string& shaderscreenvert, const std::string& shaderscreenfrag,
              const std::string shaderblackrectvert = std::string(), const std::string shaderblackrectfrag=std::string());
    void render(const Camera& cam, const cv::cuda::GpuMat& frame);

private:
    std::string proj_type;  // 投影模型类型
    ConfigProjModel proj_cfg;  // 投影模型配置参数结构体
    std::shared_ptr<ProjModelBase> proj_model;
    OGLBuffer OGLProjModel;
    OGLBuffer OGLBlackRect;
    OGLBuffer OGLQuadRender;
    float aspect_ratio;  // 窗口宽高比
    int32  wnd_width;
    int32  wnd_height;
    CUDA_OGL cuOgl;
    float white_luminance, tonemap_luminance;
private:
    std::vector<ObjModel> obj_models;
    std::vector<std::shared_ptr<Shader>> obj_model_shaders;
    std::vector<glm::mat4> obj_model_transformations;
    bool isInit = false;
    bool texReady;
};






