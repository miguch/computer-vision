//
// Created by Miguel Chan on 2018/11/20.
//

#include "spherical_warp.h"
#include "utils.h"
#include <cmath>

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif


spherical_warp::spherical_warp(const CImg<unsigned char> &src) : src(src) {}

const double angle_unit = 15.0;


CImg<unsigned char> spherical_warp::run_warping() {
    CImg<unsigned char> result(src.width(), src.height(), 1, src.spectrum(), 0);

    int width = src.width();
    int height = src.height();

    double r = (width / 2.0) / tan(angle_unit * M_PI / 180.0);

    cimg_forXY(src, x, y) {
            double x_diff = x - width / 2;
            double y_diff = y - height / 2;

            double k = r / sqrt(r * r + x_diff * x_diff);
            double src_x = x_diff / k;
            double src_y = y_diff / k;

            if (src_x + width / 2 >= 0 && src_x + width / 2 < src.width()
                && src_y + height / 2 >= 0 && src_y + height / 2 < src.height()) {
                for (int z = 0; z < src.spectrum(); z++) {
                    result(x, y, z) = utils::interpolate(src, src_x + width / 2, src_y + height / 2, z);
                }
            }

        }

    return result;
}
