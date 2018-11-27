//
// Created by Miguel Chan on 2018/11/27.
//

#include "utils.h"

CImg<uint8_t> utils::toGreyScale(const CImg<uint8_t> &src) {
    CImg<uint8_t> result(src.width(), src.height());
    cimg_forXY(src, x, y) {
            auto r = src(x, y, 0), g = src(x, y, 1), b = src(x, y, 2);
            result(x, y) = 0.299 * r + 0.587 * g + 0.114 * b;
    }
    return result;
}
