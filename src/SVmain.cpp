#include <csignal>
#include <chrono>
#include <unistd.h>
#include <stdlib.h>
#include <omp.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ThreadPool.hpp>
#include <SVDisplay.hpp>
#include <SVUtil/SVUtil.hpp>

bool finish = false;

int main(int argc, char* argv[])
{           
    SVLogger::getInstance().setLogLevel(LogLevel::DEBUG);
    ThreadPool threadpool(SVConfig::get().num_pool_threads);
    std::shared_ptr<SVRender> renderer;
    std::shared_ptr<SVDisplayView> display_viewer;  

    // 系统初始化
    display_viewer = std::make_shared<SVDisplayView>();
    renderer = std::make_shared<SVRender>(SVConfig::get().gl_width, SVConfig::get().gl_height);
    display_viewer->init(SVConfig::get().gl_width, SVConfig::get().gl_height, renderer);
    renderer->init(SVConfig::get().cfg_proj, SVConfig::get().surroundshadervert, SVConfig::get().surroundshaderfrag,
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
            break;
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
    
	return 0;

}