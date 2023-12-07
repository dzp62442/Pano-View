#pragma once
#include <memory>
#include <stdint.h>

#include <SVRender.hpp>

#define GLFW_INCLUDE_ES32
#include "GLFW/glfw3.h"

using int32 = int32_t;

// 第一项控制视点位置，第二项控制浏览时的视野旋转轴
// 第一项中y表示视点高度，设为1.7-1.9，调节x和z来控制视点在车身周围的位置
// x控制视点在车身前后，z控制视点在车身左右
static Camera cam(glm::vec3(0.5, 1.9, 0.), glm::vec3(0.0, 1.0, 0.0));  // 观察视点


class SVDisplayView
{
private:
	GLFWwindow* window;
        int32 width, height;
        float aspect_ratio;
        std::shared_ptr<SVRender> disp_view;
        bool isInit;
        bool useDemoSurroundView, useDemoTopView;

protected:
        void demoSVMode(Camera& camera);
        void demoTVMode(Camera& camera);
public:
        SVDisplayView() : window(nullptr), disp_view(nullptr)
        {
            width = 0;
            height = 0;
            isInit = false;
            useDemoSurroundView = useDemoTopView = false;
        }

        ~SVDisplayView(){glfwTerminate();}

        bool init(const int32 wnd_width, const int32 wnd_height, std::shared_ptr<SVRender> scene_view);

        bool render(const cv::cuda::GpuMat& frame);

public:
        void setSVMode(const bool demo);
        bool getSVMode() const;
        void setTVMode(const bool topview);
        bool getTVMode() const;
        void resetCameraState();
};




