#pragma once
#include <vector>
#include <functional>
#include "screen.h"

template <typename T>
std::vector<T> surrounding_tiles(int x, int y, int width, int height, const std::function<T(int, int)>& op) {
    std::vector<T> surrounding;
    surrounding.reserve(8);

    const int start_i = std::max(0, y - 1);
    const int end_i = std::min(height - 1, y + 1);
    const int start_j = std::max(0, x - 1);
    const int end_j = std::min(width - 1, x + 1);

    for (int i = start_i; i <= end_i; i++) {
        for (int j = start_j; j <= end_j; j++) {
            surrounding.push_back(op(j, i));
        }
    }

    return surrounding;
}


bool color_in_range(const Pixel& a, const Pixel& b, int range = 10);

double get_color_distance(const Pixel& a, const Pixel& b);