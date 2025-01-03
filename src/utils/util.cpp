#include <cmath>

#include "util.h"

bool color_in_range(const Pixel& a, const Pixel& b, int range) {
    return abs(a.red - b.red) <= range &&
        abs(a.green - b.green) <= range &&
        abs(a.blue - b.blue) <= range;
}

double get_color_distance(const Pixel& a, const Pixel& b) {
    int dr = static_cast<int>(a.red) - b.red;
    int dg = static_cast<int>(a.green) - b.green;
    int db = static_cast<int>(a.blue) - b.blue;
    return std::sqrt(dr * dr + dg * dg + db * db);
};