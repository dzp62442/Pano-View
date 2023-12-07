#include <SVApp.hpp>

#include <csignal>
#include <chrono>
#include <unistd.h>
#include <stdlib.h>
#include <omp.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

bool finish = false;

// 展示拼接好的全景图，2D 或 3D
bool SVApp::showPanorama()
{
    // 系统初始化
    renderer = std::make_shared<SVRender>(GLSize.width, GLSize.height);
    display_viewer = std::make_shared<SVDisplayView>();
    display_viewer->init(GLSize.width, GLSize.height, renderer);
    renderer->init(SVConfig::get().cbowl, SVConfig::get().surroundshadervert, SVConfig::get().surroundshaderfrag,
                    SVConfig::get().screenshadervert, SVConfig::get().screenshaderfrag,
                    SVConfig::get().blackrectshadervert, SVConfig::get().blackrectshaderfrag);
    if (!renderer->addModel(SVConfig::get().car_model, SVConfig::get().car_vert_shader, SVConfig::get().car_frag_shader))
        LOG_ERROR("SVApp::showPanorama", "Error can't add model");

    cv::Mat panorama;
    int imgNum = SVConfig::get().video_start;   
    std::string panorama_path = SVConfig::get().video_dir;
    panorama = cv::imread(panorama_path + std::to_string(imgNum) + ".jpg");
    cv::Size panoramaSize = cv::Size(panorama.cols, panorama.rows);  // 全景图尺寸需保持一致

    while(!finish)  // 开始浏览
    {
        SVTimer show_timer;

        // 读取全景图
        panorama = cv::imread(panorama_path + std::to_string(imgNum) + ".jpg");
        if (panorama.rows == 0 || panorama.cols == 0){
            LOG_DEBUG("SVApp::showPanorama", "WARNING! Read empty image");
            imgNum++;
            continue;
        }
        else if (panorama.cols != panoramaSize.width || panorama.rows != panoramaSize.height)
            cv::resize(panorama, panorama, panoramaSize);
        show_timer.update("read panorama");

        // 渲染全景图
        cv::cuda::GpuMat panorama_gpu;
        panorama_gpu.upload(panorama);
        renderer->setWhiteLuminance(1.0);
        renderer->setToneLuminance(1.0);
        if (!display_viewer->render(panorama_gpu)) {
            LOG_ERROR("SVApp::showPanorama", "Error render");
            return false;
        }
        show_timer.update("show");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 毫秒
        show_timer.update("sleep");

        std::cout << imgNum << "\n";
        imgNum ++;
        if (imgNum > SVConfig::get().video_stop)
            imgNum = SVConfig::get().video_start;
        show_timer.print("SVApp::showPanorama");
    }

    return true;
}
