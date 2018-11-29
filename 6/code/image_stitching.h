//
// Created by Miguel Chan on 2018/11/28.
//

#ifndef PANORAMA_IMAGE_STITCHING_H
#define PANORAMA_IMAGE_STITCHING_H

#include "CImg.h"
#include <vector>

using namespace cimg_library;

namespace stitching {
    CImg<unsigned char> run_stitching(std::vector<CImg<unsigned char>> sources);
}


#endif //PANORAMA_IMAGE_STITCHING_H
