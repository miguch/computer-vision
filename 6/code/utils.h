//
// Created by Miguel Chan on 2018/11/27.
//

#ifndef PANORAMA_UTILS_H
#define PANORAMA_UTILS_H

#include "CImg.h"
#include <cstdint>

using namespace cimg_library;

namespace utils {

    unsigned char interpolate(const CImg<unsigned char> &src, double x, double y, int channel);
    CImg<unsigned char> toGreyScale(const CImg<unsigned char>& src);

};


#endif //PANORAMA_UTILS_H
