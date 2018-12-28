//
// Created by Miguel Chan on 2018/12/26.
//

#ifndef SEGMENT_TOOLS_H
#define SEGMENT_TOOLS_H

#include "src/CImg.h"
#include <vector>

using namespace cimg_library;

namespace segmentTools {

    std::vector<std::vector<CImg<unsigned char>>> getSegments(const CImg<unsigned char> &src);
}


#endif //RECOGNITION_TOOLS_H
