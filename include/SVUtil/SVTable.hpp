#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

// 双向查找表
template<typename T>
struct BidirectionalLookupTable {
    std::vector<T> col1;
    std::vector<T> col2;

    // 向表中添加一行数据
    void addElement(T value1, T value2) {
        col1.push_back(value1);
        col2.push_back(value2);
    }

    // 给定 col1 中的值，查找 col2 中的值（必要时进行内插法计算）
    T lookupCol2FromCol1(T value1) const {
        auto it = std::lower_bound(col1.begin(), col1.end(), value1);  // 查找第一个大于等于value1的元素
        if (it == col1.begin()) {
            return col2.front();  // 找到的元素是第一列的第一个元素，则直接返回第二列的值
        } else if (it == col1.end()) {
            return col2.back();  // 找到的元素是第一列的最后一个元素，则直接返回第二列的值
        } else {  // 进行线性插值得到近似值
            auto prev_it = std::prev(it);  // 获取一个迭代器的前一个位置
            T x1 = *prev_it;
            T x2 = *it;
            T y1 = col2[std::distance(col1.begin(), prev_it)];
            T y2 = col2[std::distance(col1.begin(), it)];
            return y1 + (value1 - x1) * (y2 - y1) / (x2 - x1);
        }
    }

    // 给定 col2 中的值，查找 col1 中的值（必要时进行内插法计算）
    T lookupCol1FromCol2(T value2) const {
        auto it = std::lower_bound(col2.begin(), col2.end(), value2);  // 查找第一个大于等于value2的元素
        if (it == col2.begin()) {
            return col1.front(); // 找到的元素是第二列的第一个元素，则直接返回第一列的值
        } else if (it == col2.end()) {
            return col1.back(); // 找到的元素是第二列的最后一个元素，则直接返回第一列的值
        } else {  // 进行线性插值得到近似值
            auto prev_it = std::prev(it);  // 获取一个迭代器的前一个位置
            T x1 = *prev_it;
            T x2 = *it;
            T y1 = col1[std::distance(col2.begin(), prev_it)];
            T y2 = col1[std::distance(col2.begin(), it)];
            return y1 + (value2 - x1) * (y2 - y1) / (x2 - x1);
        }
    }

    // 清零函数，将表格清空
    void clear() {
        col1.clear();
        col2.clear();
    }
};