#pragma once
#include <meshgrid.hpp>

#include <stdint.h>
#include <limits>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

#include <SVUtil/SVUtil.hpp>

using uint = uint32_t;
using int32 = int32_t;
constexpr static float default_center[3]{0.f}; // for default value pass to constructor - argument center

//! ---------------------------------------------- 投影模型基类 ----------------------------------------------
class ProjModelBase
{
protected:  // 静态的编译时常量
    constexpr static float eps_uv = 1e-5f;  // UV 映射的精度
    constexpr static float PI = 3.14159265359f;  // 圆周率
    constexpr static auto epsilon = std::numeric_limits<float>::epsilon();  // 浮点数精度，其值为 float 类型能表示的的最小正数
    constexpr static int32 _num_vertices = 3; // x, y, z
    constexpr static float polar_coord = 2 * PI;  // 极坐标的角度范围，定义碗的全角度范围
    constexpr static float half_pi = 3.14159265359f / 2.0f;  // 半圆周率

protected:  // 模型基本参数
    float cen[3];  // 模型的中心点坐标
    bool set_hole = false;  // 是否在模型底面中心设置一个洞
    bool useUV = false;  // 是否使用 UV 映射

public:  // 构造函数
    ProjModelBase() {}

public:  // 用户调用的模型生成函数
    bool generate_mesh(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices)
    {
        useUV = false;
        return generate_mesh_(max_size_vert, vertices, indices);
    }
    bool generate_mesh_uv(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices)
    {
        useUV = true;
        return generate_mesh_(max_size_vert, vertices, indices);
    }

protected:  // 具体实现模型生成的纯虚函数
    virtual bool generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices) = 0;
    virtual void generate_indices_(std::vector<uint>& indices, const uint grid_size, const uint idx_min_y, const int32 last_vert) = 0;

protected:  // 辅助函数
    /*
        比较给定点 (x, z) 与碗中心点 cen 的距离与指定半径的关系
    */
    // 判断点 (x, z) 是否位于以 cen 为中心、指定 radius 为半径的圆内（包括边界）
    bool lt_radius(const float x, const float z, const float radius) {
        auto r1 = pow((x - cen[0]), 2);
        auto r2 = pow((z - cen[2]), 2);
        auto lt = ((r1 + r2) <= pow(radius, 2));  // 是否位于圆内
        return lt;
    }
    // 函数判断点 (x, z) 是否位于以 cen 为中心、指定 radius 为半径的圆外
    bool gt_radius(const float x, const float z, const float radius) {
        auto r1 = pow((x - cen[0]), 2);
        auto r2 = pow((z - cen[2]), 2);
        auto gt = ((r1 + r2) > pow(radius, 2));  // 是否位于圆外
        return gt;
    }

};


//! ---------------------------------------------- 抛物面碗模型 ----------------------------------------------
class BowlParabModel : public ProjModelBase
{
protected:  // 模型参数
    float hole_rad;  // 模型底面中心的洞的半径
    float inner_rad;  // 内半径
    float rad;  // 外半径
    float param_a, param_b, param_c;

public:  // 构造函数
    BowlParabModel() {}
    BowlParabModel(const ConfigProjModel& proj_cfg, const float center[3] = default_center) : 
        inner_rad(proj_cfg.inner_radius), rad(proj_cfg.radius), hole_rad(proj_cfg.hole_radius),
        param_a(proj_cfg.a), param_b(proj_cfg.b), param_c(proj_cfg.c)
    {
        cen[0] = center[0];
        cen[1] = center[1];
        cen[2] = center[2];

        // 是否在模型底面中心设置一个洞
        if (hole_rad > 0) {
            set_hole = true;
        }
        else {
            set_hole = false;
            hole_rad = 0;
        }
    }

protected:  // 具体实现模型生成
    bool generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices) override;
    void generate_indices_(std::vector<uint>& indices, const uint grid_size, const uint idx_min_y, const int32 last_vert) override;

};


//! ---------------------------------------------- 水滴模型 ----------------------------------------------
class Drops1Model : public ProjModelBase
{
protected:  // 模型参数
    float max_height;  // 最大高度
    float rad_at_base;  // 底部的最大半径
    float hole_rad;  // 模型底面中心的洞的半径

public:  // 构造函数
    Drops1Model() {}
    Drops1Model(const ConfigProjModel& proj_cfg, const float center[3] = default_center) : 
        max_height(proj_cfg.max_height), rad_at_base(proj_cfg.rad_at_base), hole_rad(proj_cfg.hole_radius)
    {
        cen[0] = center[0];
        cen[1] = center[1];
        cen[2] = center[2];

        // 是否在模型底面中心设置一个洞
        if (hole_rad > 0) {
            set_hole = true;
        }
        else {
            set_hole = false;
            hole_rad = 0;
        }
    }

protected:  // 具体实现模型生成
    bool generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices) override;
    void generate_indices_(std::vector<uint>& indices, const uint grid_size, const uint idx_min_y, const int32 last_vert) override;

};


//! ---------------------------------------------- 半球面模型 ----------------------------------------------
class HemiSphereModel : public ProjModelBase
{
protected:  // 模型参数
    int x_segment, y_segment;

public:  // 构造函数
    HemiSphereModel(const ConfigProjModel& proj_cfg, const float center[3] = default_center)
            : x_segment(proj_cfg.x_segment), y_segment(proj_cfg.y_segment)
    {
        cen[0] = center[0];
        cen[1] = center[1];
        cen[2] = center[2];
    }

protected:  // 具体实现模型生成
    bool generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices) override;
    void generate_indices_(std::vector<uint>& indices, const uint grid_size, const uint idx_min_y, const int32 last_vert) override;
};


//! ---------------------------------------------- 球面模型 ----------------------------------------------
class SphereModel : public ProjModelBase
{
protected:  // 模型参数
    int x_segment, y_segment;

public:  // 构造函数
    SphereModel(const ConfigProjModel& proj_cfg, const float center[3] = default_center)
            : x_segment(proj_cfg.x_segment), y_segment(proj_cfg.y_segment)
    {
        cen[0] = center[0];
        cen[1] = center[1];
        cen[2] = center[2];
    }

protected:  // 具体实现模型生成
    bool generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices) override;
    void generate_indices_(std::vector<uint>& indices, const uint grid_size, const uint idx_min_y, const int32 last_vert) override;
};
