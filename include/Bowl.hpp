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

class Bowl
{
private:
    constexpr static float eps_uv = 1e-5f;
    constexpr static float PI = 3.14159265359f;
    constexpr static auto epsilon = std::numeric_limits<float>::epsilon();
    constexpr static int32 _num_vertices = 3; // x, y, z
private:
    float cen[3];
    float inner_rad;
    float rad;
    float param_a, param_b, param_c;
    float hole_rad;
    bool set_hole = false;
    bool useUV = false;
    float polar_coord = 2 * PI;
public:
    Bowl(const float inner_radius, const float radius, const float a, const float b, const float c, const float center[3] = default_center)
            : inner_rad(inner_radius), rad(radius), param_a(a), param_b(b), param_c(c), hole_rad(0.0f)
    {
            cen[0] = center[0];
            cen[1] = center[1];
            cen[2] = center[2];
    }
    Bowl(const ConfigBowl& cbowl, const float center[3] = default_center) : inner_rad(cbowl.disk_radius), rad(cbowl.parab_radius),
        param_a(cbowl.a), param_b(cbowl.b), param_c(cbowl.c), hole_rad(0.0f)
    {
        cen[0] = center[0];
        cen[1] = center[1];
        cen[2] = center[2];
    }

    bool generate_mesh(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices)
    {
            set_hole = false;
            useUV = false;
            polar_coord = 2 * PI;
            return generate_mesh_(max_size_vert, vertices, indices);
    }
    bool generate_mesh_uv(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices)
    {
            set_hole = false;
            useUV = true;
            polar_coord = 2 * PI;
            return generate_mesh_(max_size_vert, vertices, indices);
    }


    bool generate_mesh_hole(const float max_size_vert, const float hole_radius, std::vector<float>& vertices, std::vector<uint>& indices)
    {
            set_hole = true;
            useUV = false;
            hole_rad = hole_radius;
            polar_coord = 2 * PI;
            return generate_mesh_(max_size_vert, vertices, indices);
    }

    bool generate_mesh_uv_hole(const float max_size_vert, const float hole_radius, std::vector<float>& vertices, std::vector<uint>& indices)
    {
            set_hole = true;
            useUV = true;
            hole_rad = hole_radius;
            polar_coord = 2 * PI;
            return generate_mesh_(max_size_vert, vertices, indices);
    }

protected:
    bool generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices);

private:
    void generate_indices(std::vector<uint>& indices, const uint grid_size, const uint idx_min_y, const int32 last_vert);

    // compare inner radius and outer radius
    bool lt_radius(const float x, const float z, const float radius) {
            auto r1 = pow((x - cen[0]), 2);
            auto r2 = pow((z - cen[2]), 2);
            auto lt = ((r1 + r2) <= pow(radius, 2));
            return lt;
    }
    bool gt_radius(const float x, const float z, const float radius) {
            auto r1 = pow((x - cen[0]), 2);
            auto r2 = pow((z - cen[2]), 2);
            auto gt = ((r1 + r2) > pow(radius, 2));
            return gt;
    }
};



class HemiSphere
{
private:
    constexpr static float eps_uv = 1e-4f;
    constexpr static float PI = 3.14159265359f;
    constexpr static auto epsilon = std::numeric_limits<float>::epsilon();
    constexpr static int32 _num_vertices = 3; // x, y, z
private:
    float cen[3];
    float hole_rad;
    bool set_hole = false;
    float polar_coord = 2 * PI;
    float half_pi = PI / 2.0f;
    int x_segment, y_segment;
public:
    HemiSphere(const int x_segm, const int y_segm, const float center[3] = default_center)
            : x_segment(x_segm), y_segment(y_segm), hole_rad(0.0f)
    {
            cen[0] = center[0];
            cen[1] = center[1];
            cen[2] = center[2];
    }
    bool generate_mesh_uv(std::vector<float>& vertices, std::vector<uint>& indices)
    {
            polar_coord = 2 * PI;
            return generate_mesh_(vertices, indices);
    }

protected:
    bool generate_mesh_(std::vector<float>& vertices, std::vector<uint>& indices);

};
