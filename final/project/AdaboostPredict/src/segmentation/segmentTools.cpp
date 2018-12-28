//
// Created by Miguel Chan on 2018/12/26.
//

#include "segmentTools.h"

#include "segmentation.h"

std::vector<std::vector<CImg<unsigned char>>> segmentTools::getSegments(const CImg<unsigned char> &src) {
    segmentation segments(src);

    return segments.runSegmentation();
}
