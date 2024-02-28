#include <csignal>
#include <chrono>
#include <unistd.h>
#include <stdlib.h>
#include <omp.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ThreadPool.hpp>
#include <ProjModel.hpp>
#include <SVDisplay.hpp>
#include <SVUtil/SVUtil.hpp>

bool finish = false;

int main(int argc, char* argv[])
{           
    SVLogger::getInstance().setLogLevel(LogLevel::DEBUG);
    ThreadPool threadpool(SVConfig::get().num_pool_threads);
    std::shared_ptr<SVDisplayView> display_viewer;
    std::shared_ptr<SVRender3D> renderer;
    
    // 系统初始化
    display_viewer = std::make_shared<SVDisplayView>();
    renderer = std::make_shared<SVRender3D>(SVConfig::get().gl_width, SVConfig::get().gl_height, SVConfig::get().proj_type);
    display_viewer->init(SVConfig::get().gl_width, SVConfig::get().gl_height, renderer);
    renderer->init(SVConfig::get().proj_cfg, SVConfig::get().surroundshadervert, SVConfig::get().surroundshaderfrag,
                    SVConfig::get().screenshadervert, SVConfig::get().screenshaderfrag,
                    SVConfig::get().blackrectshadervert, SVConfig::get().blackrectshaderfrag);
    if (!renderer->addObjModel(SVConfig::get().car_model, SVConfig::get().car_vert_shader, SVConfig::get().car_frag_shader))
        LOG_ERROR("SVApp::showPanorama", "Error can't add model");

    cv::Mat panorama;
    int imgNum = SVConfig::get().video_start;   
    std::string panorama_path = SVConfig::get().video_dir;
    panorama = cv::imread(panorama_path + std::to_string(imgNum) + ".jpg");
    cv::Size panoramaSize = cv::Size(panorama.cols, panorama.rows);  // 全景图尺寸需保持一致
    float top_black_height_scale = SVConfig::get().proj_cfg.top_black_height_scale;
    float bottom_black_height_scale = SVConfig::get().proj_cfg.bottom_black_height_scale;

    // 开始浏览
    while(!finish)  
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

        //! 全景图上下接入黑色矩形
        int topAdditionalHeight = static_cast<int>(panoramaSize.height * top_black_height_scale);
        int bottomAdditionalHeight = static_cast<int>(panoramaSize.height * bottom_black_height_scale);
        int newHeight = panoramaSize.height + topAdditionalHeight + bottomAdditionalHeight;
        cv::Mat newPanorama(newHeight, panoramaSize.width, panorama.type(), cv::Scalar::all(10));
        cv::Rect roi(0, topAdditionalHeight, panoramaSize.width, panoramaSize.height);
        panorama.copyTo(newPanorama(roi));

        // 渲染全景图
        cv::cuda::GpuMat panorama_gpu;
        panorama_gpu.upload(newPanorama);
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
        // imgNum ++;
        if (imgNum > SVConfig::get().video_stop)
            imgNum = SVConfig::get().video_start;
        show_timer.print("SVApp::showPanorama");
    }
    
	return 0;

}