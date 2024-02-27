#include "ProjModel.hpp"

//! ---------------------------------------------- 抛物面碗模型 ----------------------------------------------

// 生成抛物面碗状网格的顶点和索引
bool BowlParabModel::generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices)
{
    // 参数校验
    if (fabs(param_a) <= epsilon || fabs(param_b) <= epsilon || fabs(param_c) <= epsilon)
        return false;
    if (rad <= 0.f || inner_rad <= 0.f)
        return false;
    if (set_hole && hole_rad <= 0.f)
        return false;
    auto a = param_a;
    auto b = param_b;
    auto c = param_c;

    // 初始化顶点和索引数组
    vertices.clear();
    indices.clear();

    /*
        在极坐标系中准备网格：半径 r 角度 theta
    */
    // 生成纹理坐标 (u, v) [0, 1]
    std::vector<float> texture_u = meshgen::linspace(0.f, (1.f + eps_uv), max_size_vert);
    auto texture_v = texture_u;
    // 生成半径和角度值
    auto r = meshgen::linspace(hole_rad, rad, max_size_vert); // min_size = 0.f, max_size = 100.f,
    auto theta = meshgen::linspace(0.f, polar_coord, max_size_vert);  // [0, 2*PI]
    auto mesh_pair = meshgen::meshgrid(r, theta);  // 创建一个网格，每个点由一个半径值和一个角度值组成

    /*
        转化为笛卡尔坐标
    */
    // 初始化坐标向量
    auto R = std::get<0>(mesh_pair);  // 从网格对中取第一个元素，即所有网格点的半径值
    auto THETA = std::get<1>(mesh_pair);  // 从网格对中取第二个元素，即所有网格点的角度值
    size_t grid_size = R.size();
    std::vector<float> x_grid;
    std::vector<float> y_grid;
    std::vector<float> z_grid;
    // 遍历极坐标网格并进行坐标转换
    // x = r*cos(theta), z = r*sin(theta), y/c = (x^2)/(a^2) + (z^2)/(b^2);
    for (size_t i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            auto x = R(i, j) * cos(THETA(i, j));
            auto z = R(i, j) * sin(THETA(i, j));
            auto y = c * (pow((x / a), 2) + pow((z / b), 2));
            x_grid.push_back(x);
            z_grid.push_back(z);
            y_grid.push_back(y);
        }
    }

    /*
        在生成的网格中找到从圆盘（Disk）过渡到椭圆抛物面（Elliptic Paraboloid）的起始水平面
        具体来说，寻找网格中首次出现在内半径以内的点，表示这一层级是从圆盘到椭圆抛物面的转换点
    */
    auto min_y = 0.f;  // 转换点的 y 坐标值
    auto idx_min_y = 0u; // 转换点在网格中的行索引
    for (size_t i = 0; i < grid_size; ++i) {  // 外层循环遍历网格的每一行
        for (size_t j = 0; j < grid_size; ++j) {  // 内层循环遍历每一行的每个点
            auto x = x_grid[j + i * grid_size];
            auto z = z_grid[j + i * grid_size];
            if (lt_radius(x, z, inner_rad)) {  // 检查点 (x, z) 是否位于内半径 inner_rad 内部
                min_y = y_grid[j + i * grid_size];  // 找到了转换层级的 y 坐标
                idx_min_y = i;
                break;
            }
        }
    }

    /*
        生成碗状网格顶点数据，包括圆盘部分和椭圆抛物面部分
    */
    auto half_grid = grid_size / 2;  // 网格尺寸的一半，用于纹理坐标计算
    auto vertices_size = 0;  // 用于记录顶点数组的大小
    auto offset_idx_min_y = 0;
    for (size_t i = 0; i < grid_size; ++i) {
        for (size_t j = 0; j < grid_size; ++j) {
            auto x = x_grid[j + i * grid_size];
            auto z = z_grid[j + i * grid_size];

            auto y = min_y;  // y 坐标初始化为 min_y，圆盘和抛物面交界处的高度
            if (gt_radius(x, z, inner_rad))  // 若点 (x, z) 在内半径之外
                y = y_grid[j + i * grid_size];  // y 坐标更新为对应的 y_grid 值，表示点位于抛物面上

            vertices.push_back(x + cen[0]);
            vertices.push_back(y + cen[1]);
            vertices.push_back(z + cen[2]);
            vertices_size += 3;  // 添加顶点到顶点数组

            if (useUV) {  // 处理纹理坐标（如果需要）
                auto u = texture_u[j];  // 从 texture_u 向量中获取当前列 j 的 u 纹理坐标
                auto v = texture_v[i];  // 从 texture_v 向量中获取当前行 i 的 v 纹理坐标
                if (i == 0 && j == 0 && !set_hole)  // 当处理网格的中心点（即第一行第一列，i 和 j 都为 0）且没有设置中心洞（!set_hole）时，进行特殊处理
                    u = texture_u[half_grid];  // 将 u 坐标设置为 texture_u 中间点的值，这是为了调整中心点的纹理坐标
                vertices.push_back(u);
                vertices.push_back(v);
            }
        }
    }


    /*
        生成网格的索引，用于确定如何将顶点组合成三角形
    */
    idx_min_y -= offset_idx_min_y;
    int32 last_vert = vertices_size / _num_vertices;  // 顶点数组中最后一个顶点的索引
    generate_indices_(indices, grid_size, idx_min_y, last_vert);

    return true;
}

// 生成抛物面碗状网格的索引，索引决定了如何将顶点组合成三角形以构成网格
void BowlParabModel::generate_indices_(std::vector<uint>& indices, const uint grid_size, const uint idx_min_y, const int32 last_vert) 
{
    bool oddRow = false;  // 判断当前行是奇数行还是偶数行

    for (uint y = 0; y < grid_size - 1; ++y) {  // 遍历网格的每一行，除了最后一行（因为它没有下一行来形成三角形）
        if (!oddRow) {  // 偶数行: y == 0, y == 2; and so on
            for (uint x = 0; x < grid_size; ++x) {
                auto current = y * grid_size + x;
                auto next = (y + 1) * grid_size + x;
                /* change order when change disk to elliptic paraboloid */
                if (y == idx_min_y && x == 0) {
                    std::swap(current, next);
                    indices.push_back(current - grid_size);
                    indices.push_back(next);
                    indices.push_back(current);
                    continue;
                }
                if (set_hole && (current >= last_vert || next >= last_vert))
                    continue;
                indices.push_back(current);
                indices.push_back(next);
            }
        }
        else {  // 奇数行: y == 1, y == 3; and so on
            for (int x = grid_size - 1; x >= 0; --x) {
                auto current = (y + 1) * grid_size + x;
                auto prev = y * grid_size + x;
                /* change order when change disk to elliptic paraboloid */
                if (y == idx_min_y && x == grid_size - 1) {
                    indices.push_back(current - grid_size);
                    indices.push_back(current);
                    indices.push_back(prev);
                    continue;
                }
                if (set_hole && (current >= last_vert || prev >= last_vert))
                    continue;
                indices.push_back(current);
                indices.push_back(prev);
            }
        }
        oddRow = !oddRow;
    }
}


//! ---------------------------------------------- 水滴模型 ----------------------------------------------

// 生成抛物面碗状网格的顶点和索引
bool Drops1Model::generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices)
{
    // 参数校验
    if (fabs(max_height) <= epsilon || fabs(rad_at_base) <= epsilon)
        return false;
    if (set_hole && hole_rad <= 0.f)
        return false;

    // 初始化顶点和索引数组
    vertices.clear();
    indices.clear();

    /*
        在极坐标系中准备网格：半径 r 角度 theta
    */
    // 生成纹理坐标 (u, v) [0, 1]
    std::vector<float> texture_u = meshgen::linspace(0.f, (1.f + eps_uv), max_size_vert);
    auto texture_v = texture_u;
    // 生成高度和角度值
    auto ts = meshgen::linspace(1.0f, 0.0f, max_size_vert);
    auto theta = meshgen::linspace(0.f, polar_coord, max_size_vert);  // [0, 2*PI]
    auto mesh_pair = meshgen::meshgrid(ts, theta);  // 创建一个网格，每个点由一个高度值和一个角度值组成

    /*
        转化为笛卡尔坐标
    */
    // 初始化坐标向量
    auto T = std::get<0>(mesh_pair);  // 从网格对中取第一个元素，即所有网格点的高度值
    auto THETA = std::get<1>(mesh_pair);  // 从网格对中取第二个元素，即所有网格点的角度值
    size_t grid_size = T.size();
    std::vector<float> x_grid;
    std::vector<float> y_grid;
    std::vector<float> z_grid;
    // 遍历极坐标网格并进行坐标转换
    for (size_t i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            auto t = T(i, j);
            auto r = rad_at_base * (1 - t) * t;
            auto x = r * cos(THETA(i, j));
            auto z = r * sin(THETA(i, j));
            auto y = max_height * (pow(t, 4) - 4 * t + 3);
            x_grid.push_back(x);
            z_grid.push_back(z);
            y_grid.push_back(y);
        }
    }

    /*
        生成碗状网格顶点数据，包括圆盘部分和椭圆抛物面部分
    */
    auto half_grid = grid_size / 2;  // 网格尺寸的一半，用于纹理坐标计算
    auto vertices_size = 0;  // 用于记录顶点数组的大小
    auto offset_idx_min_y = 0;
    for (size_t i = 0; i < grid_size; ++i) {
        for (size_t j = 0; j < grid_size; ++j) {
            auto x = x_grid[j + i * grid_size];
            auto z = z_grid[j + i * grid_size];
            auto y = y_grid[j + i * grid_size];

            vertices.push_back(x + cen[0]);
            vertices.push_back(y + cen[1]);
            vertices.push_back(z + cen[2]);
            vertices_size += 3;  // 添加顶点到顶点数组

            if (useUV) {  // 处理纹理坐标（如果需要）
                auto u = texture_u[j];  // 从 texture_u 向量中获取当前列 j 的 u 纹理坐标
                auto v = texture_v[i];  // 从 texture_v 向量中获取当前行 i 的 v 纹理坐标
                if (i == 0 && j == 0 && !set_hole)  // 当处理网格的中心点（即第一行第一列，i 和 j 都为 0）且没有设置中心洞（!set_hole）时，进行特殊处理
                    u = texture_u[half_grid];  // 将 u 坐标设置为 texture_u 中间点的值，这是为了调整中心点的纹理坐标
                vertices.push_back(u);
                vertices.push_back(v);
            }
        }
    }


    /*
        生成网格的索引，用于确定如何将顶点组合成三角形
    */
    int32 last_vert = vertices_size / _num_vertices;  // 顶点数组中最后一个顶点的索引
    generate_indices_(indices, grid_size, 0, last_vert);

    return true;
}

// 生成抛物面碗状网格的索引，索引决定了如何将顶点组合成三角形以构成网格
void Drops1Model::generate_indices_(std::vector<uint>& indices, const uint grid_size, const uint idx_min_y, const int32 last_vert) 
{
    bool oddRow = false;  // 判断当前行是奇数行还是偶数行

    for (uint y = 0; y < grid_size - 1; ++y) {  // 遍历网格的每一行，除了最后一行（因为它没有下一行来形成三角形）
        if (!oddRow) {  // 偶数行: y == 0, y == 2; and so on
            for (uint x = 0; x < grid_size; ++x) {
                auto current = y * grid_size + x;
                auto next = (y + 1) * grid_size + x;
                indices.push_back(current);
                indices.push_back(next);
            }
        }
        else {  // 奇数行: y == 1, y == 3; and so on
            for (int x = grid_size - 1; x >= 0; --x) {
                auto current = (y + 1) * grid_size + x;
                auto prev = y * grid_size + x;
                indices.push_back(current);
                indices.push_back(prev);
            }
        }
        oddRow = !oddRow;
    }
}


//! ---------------------------------------------- 半球面模型 ----------------------------------------------

// 生成半球面网格的顶点和索引
bool HemiSphereModel::generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices)
{
    vertices.clear();
    indices.clear();

    // 生成顶点，每个顶点由其空间坐标（xPos, yPos, zPos）和纹理坐标（xSegm, ySegm）组成
    for(int y = 0; y <= y_segment; ++y) {  // 遍历半球体网格的纬度
        for(int x = 0; x <= x_segment; ++x) {  // 遍历半球体网格的经度
            // 计算当前顶点在纹理图像上的位置
            float xSegm = (float)x / (float)x_segment;
            float ySegm = (float)y / (float)y_segment;
            //使用球面坐标到笛卡尔坐标的转换公式计算顶点的空间坐标
            float xPos = std::cos(xSegm * 2.0 * PI) * std::sin(ySegm * half_pi);
            float yPos = 1.0f - std::cos(ySegm * half_pi);
            float zPos = std::sin(xSegm * 2.0 * PI) * std::sin(ySegm * half_pi);
            // 添加顶点数据到向量
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            vertices.push_back(xSegm);
            vertices.push_back(ySegm);
        }
    }

    // 生成网格的索引，用于确定如何将顶点组合成三角形
    generate_indices_(indices, 0, 0, 0);
    

    return true;
}

// 生成半球面网格的索引，索引决定了如何将顶点组合成三角形以构成网格
void HemiSphereModel::generate_indices_(std::vector<uint>& indices, const uint grid_size, const uint idx_min_y, const int32 last_vert)
{
    // 生成索引
    bool oddRow = false;
    for(uint y = 0; y < y_segment - 1; ++y) {
        if (!oddRow) {
            for(uint x = 0; x <= x_segment; ++x) {
                indices.push_back(y * (x_segment + 1) + x);
                indices.push_back((y + 1)* (x_segment + 1) + x);
            }
        }
        else {
            for(int x = x_segment; x >= 0; --x) {
                indices.push_back((y + 1)* (x_segment + 1) + x);
                indices.push_back(y * (x_segment + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

}


//! ---------------------------------------------- 球面模型 ----------------------------------------------

// 生成球面网格的顶点和索引
bool SphereModel::generate_mesh_(const float max_size_vert, std::vector<float>& vertices, std::vector<uint>& indices)
{
    vertices.clear();
    indices.clear();

    // 生成顶点，每个顶点由其空间坐标（xPos, yPos, zPos）和纹理坐标（xSegm, ySegm）组成
    for(int y = 0; y <= y_segment; ++y) {  // 遍历半球体网格的纬度
        for(int x = 0; x <= x_segment; ++x) {  // 遍历半球体网格的经度
            // 计算当前顶点在纹理图像上的位置
            float xSegm = (float)x / (float)x_segment;
            float ySegm = (float)y / (float)y_segment;
            //使用球面坐标到笛卡尔坐标的转换公式计算顶点的空间坐标
            float xPos = std::cos(xSegm * 2.0 * PI) * std::sin(ySegm * PI);
            float yPos = - std::cos(ySegm * PI);
            float zPos = std::sin(xSegm * 2.0 * PI) * std::sin(ySegm * PI);
            // 添加顶点数据到向量
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            vertices.push_back(xSegm);
            vertices.push_back(ySegm);
        }
    }

    // 生成网格的索引，用于确定如何将顶点组合成三角形
    generate_indices_(indices, 0, 0, 0);
    

    return true;
}

// 生成球面网格的索引，索引决定了如何将顶点组合成三角形以构成网格
void SphereModel::generate_indices_(std::vector<uint>& indices, const uint grid_size, const uint idx_min_y, const int32 last_vert)
{
    // 生成索引
    bool oddRow = false;
    for(uint y = 0; y < y_segment - 1; ++y) {
        if (!oddRow) {
            for(uint x = 0; x <= x_segment; ++x) {
                indices.push_back(y * (x_segment + 1) + x);
                indices.push_back((y + 1)* (x_segment + 1) + x);
            }
        }
        else {
            for(int x = x_segment; x >= 0; --x) {
                indices.push_back((y + 1)* (x_segment + 1) + x);
                indices.push_back(y * (x_segment + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

}
