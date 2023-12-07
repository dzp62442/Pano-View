#include <iostream>
// #include "opencv2/viz.hpp"
#include "opencv2/opencv.hpp"

class SVCamViz
{
private:
    int imgs_num;

public:
    SVCamViz(const int num_imgs) : imgs_num(num_imgs) {}
    ~SVCamViz() {}

    bool viz(const std::vector<cv::detail::CameraParams>& cameras)
    {
        if (cameras.size() != static_cast<size_t>(imgs_num))
            return false;

        // cv::viz::Viz3d myWindow("Coordinate Frame"); 
        
        // for (int i = 0; i < imgs_num; ++i) {
        //     cv::Matx33d matR = cameras[i].R;
        //     cv::Vec3d vecT = cameras[i].t;
        //     cv::Affine3d Transpose(matR, vecT);
        //     myWindow.showWidget("Cam" + std::to_string(i), cv::viz::WCoordinateSystem(), Transpose);
        // }

        // myWindow.spin();
        
        return true;        
    }

};

