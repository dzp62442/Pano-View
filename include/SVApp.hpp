#pragma once
#include <SVDisplay.hpp>
#include <ThreadPool.hpp>
#include <SVUtil/SVUtil.hpp>

class SVApp
{
private:
    int test;
    ThreadPool threadpool;
    cv::Size cameraSize;
    cv::Size dataSize;
    cv::Size GLSize;
    float warped_image_scale;  // 在 readCamParameters 函数中初始化，其值为 FocalLength
private:
    std::shared_ptr<SVRender> renderer;
    std::shared_ptr<SVDisplayView> dp;
    int count_estimate = 0;  // 位姿估计模块位姿更新计数器
    int count_stitch = 0;  // 图像拼接模块位姿更新计数器
    

public:
    SVApp();
    ~SVApp() {}

    SVApp& operator=(const SVApp&) = delete;
    SVApp(const SVApp&) = delete;

    bool showPanorama();
};
