//
// Created by Miguel Chan on 2018/12/1.
//

#ifndef PANORAMA_CYLINDRICAL_WARP_H
#define PANORAMA_CYLINDRICAL_WARP_H

#include "CImg.h"

using namespace cimg_library;

class cylindrical_warp {
private:
    const CImg<unsigned char> &src;

public:
    explicit cylindrical_warp(const CImg<unsigned char> &src);

    CImg<unsigned char> run_warping();
};


#endif //PANORAMA_SPHERICAL_WARP_H
