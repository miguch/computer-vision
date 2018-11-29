//
// Created by Miguel Chan on 2018/11/20.
//

#ifndef PANORAMA_SPHERICAL_WARP_H
#define PANORAMA_SPHERICAL_WARP_H

#include "CImg.h"

using namespace cimg_library;

class spherical_warp {
private:
    const CImg<unsigned char> &src;

public:
    explicit spherical_warp(const CImg<unsigned char> &src);

    CImg<unsigned char> run_warping();
};


#endif //PANORAMA_SPHERICAL_WARP_H
