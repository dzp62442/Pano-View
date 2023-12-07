#pragma once
#include <functional>
#include <chrono>
#include <future>
#include <cstdio>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgcodecs.hpp>
#include <SVUtil/SVLogger.hpp>
#include <SVUtil/SVTimer.hpp>
#include <SVUtil/SVCamViz.hpp>
#include <SVUtil/SVConfig.hpp>
#include <SVUtil/SVTable.hpp>

// 提供一种简单的方法来延迟执行函数，并且可以选择在另一个线程中异步执行，而不会阻塞当前线程。
class DefferedFunc
{
public:
    template<typename callable, typename... Args>
    DefferedFunc(const int time_ms, const bool use_async, callable&& func, Args&&... args)
    {
        std::function<typename std::result_of<callable(Args...)>::type()> task(std::bind(std::forward<callable>(func),
                                                                                         std::forward<Args>(args)...));
        if (use_async){
            std::thread([time_ms, task](){
                std::this_thread::sleep_for(std::chrono::microseconds(time_ms));
                task();
            }).detach();
        }
        else{
            std::this_thread::sleep_for(std::chrono::microseconds(time_ms));
            task();
        }
    }

};

// 保存 cv::cuda::GpuMat
inline void saveGpuMat(const cv::cuda::GpuMat& img, const std::string save_name)
{
    cv::Mat save_dst;
    img.download(save_dst);
    cv::imwrite(save_name, save_dst);
}

// 角度弧度转换
inline float rad2deg(float rad) { return rad * 180 / M_PI;}
inline float deg2rad(float deg) { return deg * M_PI / 180;}

// 判断旋转矩阵或旋转向量
inline int rotationType(cv::Mat& R){
    if (R.rows == 3 && R.cols == 3)
        return 9;
    else if (R.rows == 3 && R.cols == 1)
        return 3;
    else 
        LOG_ERROR("rotationType", "Rotation matrix size error!");

    return 0;
}

// 将数字转换为指定宽度的字符串，不足的部分用 0 填充，用于文件读取和保存的命名
inline std::string int2StringZeros(int value, int width = 6) {
    char buffer[width+2];
    std::snprintf(buffer, sizeof(buffer), "%0*d", width, value);
    return std::string(buffer);
}