//
// Created by Miguel Chan on 2018/11/20.
//

#include "spherical_warp.h"
#include <cmath>

#ifndef M_PI
    #define M_PI        3.14159265358979323846264338327950288
#endif

//Convert image from spherical coordinate (θ, φ) to cartesian (x, y)
CImg<unsigned char> spherical_warp::from_spherical(const CImg<double> &input, double focal) {
    CImg<unsigned char> result(input.width(), input.height(), 1, 3, 0);


    return result;
}

//Convert image from cartesian (x, y) to spherical coordinate (u,v)
CImg<double>
spherical_warp::to_spherical(const CImg<unsigned char> &input, double focal, const CImg<double> &rotate_mat) {
    CImg<double> result(input.width(), input.height(), 1, 2, 0);
    cimg_forX(result, x) {
        cimg_forY(result, y) {
            auto xf = 2 * M_PI * (double(x) - 0.5f * input.width()) / input.width();
            auto yf = M_PI * (double(y) - 0.5f * input.height()) / input.height();

            double xt = 0, yt = 0, zt = 0;

            xt = sin(xf) * cos(yf);
            yt = sin(yf);
            zt = cos(xf) * cos(yf);

            CImg<double> vec(1, 3, 1, 1, 0);
            vec(0, 0) = xt;
            vec(0, 1) = yt;
            vec(0, 2) = zt;

            vec = rotate_mat * vec;
            if (vec(0, 2) <= 0) continue;

            // xt/zt, yt/zt
            xt = vec(0, 0) / vec(0, 2);
            yt = vec(0, 1) / vec(0, 2);

            double xn = 0.5f * input.width() + xt * focal;
            double yn = 0.5f * input.height() + yt * focal;

            result(x, y, 0) = xn;
            result(x, y, 1) = yn;
        }
    }


    return result;
}
