//
// Created by Miguel Chan on 2018/12/25.
//

#ifndef A4_TOOLS_H
#define A4_TOOLS_H

#include "src/CImg.h"

using namespace cimg_library;

namespace a4Tools {

    CImg<unsigned char> adjustA4Image(const CImg<unsigned char> &img,
                                      const char *debugMessage = "",
                                      bool saveKmeans = false,
                                      bool saveRegionGrow = false,
                                      bool saveOutline = false,
                                      bool saveIntersects = false,
                                      bool saveResult = false);
}

#endif //RECOGNITION_TOOLS_H
