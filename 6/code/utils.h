//
// Created by Miguel Chan on 2018/11/27.
//

#ifndef PANORAMA_UTILS_H
#define PANORAMA_UTILS_H

#include "CImg.h"

using namespace cimg_library;

namespace utils {

    CImg<uint8_t> toGreyScale(const CImg<uint8_t>& src);

};


#endif //PANORAMA_UTILS_H
