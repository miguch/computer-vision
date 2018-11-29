//
// Created by Miguel Chan on 2018/11/27.
//

#include "utils.h"

unsigned char utils::interpolate(const CImg<unsigned char> &src, double x, double y, int channel) {
    int x1 = int(x), y1 = int(y);
    double x1_diff = x - x1, y1_diff = y - y1;
    double x2_diff = 1 - x1_diff, y2_diff = 1 - y1_diff;
    int x2 = (x1 >= src.width() - 1) ? x1 : x1 + 1;
    int y2 = (y1 >= src.height() - 1) ? y1 : y1 + 1;

    double fxy1 = x2_diff * src(x1, y1, channel) + x1_diff * src(x2, y1, channel);
    double fxy2 = x2_diff * src(x1, y2, channel) + x1_diff * src(x2, y2, channel);

    return (unsigned char)(y2_diff * fxy1 + y1_diff * fxy2);
}


CImg<unsigned char> utils::toGreyScale(const CImg<unsigned char> &src) {
    CImg<unsigned char> result(src.width(), src.height());
    cimg_forXY(src, x, y) {
            auto r = src(x, y, 0), g = src(x, y, 1), b = src(x, y, 2);
            result(x, y) = 0.299 * r + 0.587 * g + 0.114 * b;
    }
    return result;
}
