//
// Created by Miguel Chan on 2018/12/11.
//

#ifndef A4_REGIONGROW_H
#define A4_REGIONGROW_H

#include "../CImg.h"

using namespace cimg_library;

CImg<unsigned char> runRegionGrow(const CImg<unsigned char> &src, unsigned char threshold);


#endif //A4_REGIONGROW_H
