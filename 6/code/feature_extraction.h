//
// Created by Miguel Chan on 2018/11/27.
//

#ifndef PANORAMA_FEATURE_EXTRACTION_H
#define PANORAMA_FEATURE_EXTRACTION_H

#include "CImg.h"
#include <cstdint>
#include <map>
#include <array>

extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
}

using namespace cimg_library;

class feature_extraction {
private:
    const CImg<unsigned char> & src;

public:
    explicit feature_extraction(const CImg<unsigned char> &src);

    vl_sift_pix * cimgToData();

    std::map<std::array<float, 128>, VlSiftKeypoint> run_extraction();
};


#endif //PANORAMA_FEATURE_EXTRACTION_H
