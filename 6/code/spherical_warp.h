//
// Created by Miguel Chan on 2018/11/20.
//

#ifndef PANORAMA_SPHERICAL_WARP_H
#define PANORAMA_SPHERICAL_WARP_H

#include "CImg.h"

using namespace cimg_library;

class spherical_warp {
public:
    static CImg<double> to_spherical(const CImg<unsigned char> &input, double focal, const CImg<double> &rotate_mat);

    static CImg<unsigned char> from_spherical(const CImg<double> &input, double focal);
};


#endif //PANORAMA_SPHERICAL_WARP_H
