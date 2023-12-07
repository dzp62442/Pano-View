#pragma once
#include <SVDisplay.hpp>
#include <ThreadPool.hpp>
#include <SVUtil/SVUtil.hpp>

class SVApp
{
private:
    int test;
    ThreadPool threadpool;
    cv::Size GLSize;
private:
    std::shared_ptr<SVRender> renderer;
    std::shared_ptr<SVDisplayView> display_viewer;
    int count_estimate = 0;  // 位姿估计模块位姿更新计数器
    int count_stitch = 0;  // 图像拼接模块位姿更新计数器
    

public:
    SVApp(): GLSize(SVConfig::get().gl_width, SVConfig::get().gl_height), threadpool(SVConfig::get().num_pool_threads) {}
    ~SVApp() {}

    SVApp& operator=(const SVApp&) = delete;
    SVApp(const SVApp&) = delete;

    bool showPanorama();
};
