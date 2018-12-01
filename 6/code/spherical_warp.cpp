//
// Created by Miguel Chan on 2018/11/20.
//

#include "spherical_warp.h"

#include <cmath>
#include "utils.h"

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif

spherical_warp::spherical_warp(const CImg<unsigned char> &src) : src(src) {

}

CImg<unsigned char> spherical_warp::run_warping() {

    double min_len = fmin(src.height(), src.width());
    double radius1 = min_len / 2;
    double radius2 = pow(radius1, 2);
    double middle = radius1 * 2 / M_PI;

    CImg<unsigned char> result(src.width(), src.height(), 1, src.spectrum(), 0);

    cimg_forXY(result, x, y) {
        double rx = x - src.width() / 2;
        double ry = y - src.height() / 2;
        if (pow(rx, 2) + pow(ry, 2) >= radius2) {
            continue;
        }
        double srcX, srcY, oa, angle;
        if (rx != 0) {
            oa = middle * asin(sqrt(pow(rx, 2) + pow(ry, 2)) / radius1);
            angle = atan2(ry, rx);
            srcX = cos(angle) * oa;
            srcY = sin(angle) * oa;
        } else {
            srcX = 0;
            srcY = asin(ry / radius1) * middle;
        }
        for (int i = 0; i < src.spectrum(); i++) {
            result(x, y, i) = utils::interpolate(src, srcX + src.width() / 2, src.height() / 2 + srcY, i);
        }
    }
    return result;

}
