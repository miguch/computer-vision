//
// Created by Miguel Chan on 2018/11/26.
//

#ifndef PANORAMA_FEATURE_EXTRACT_H
#define PANORAMA_FEATURE_EXTRACT_H

#include <vector>
#include "CImg.h"


using namespace cimg_library;

//Sift
class feature_extract {
private:
    const CImg<uint8_t > & src;
    std::vector<CImg<uint8_t>> scales;
    std::vector<CImg<int>> DOGs;
    std::vector<CImg<uint8_t>> extremas;
    std::vector<CImg<double>> magnitudes;
    std::vector<CImg<double>> orients;

public:
    feature_extract(const CImg<uint8_t> & src);

    std::vector<CImg<uint8_t>> getOctaves();

    std::vector<CImg<int>> getDOGs();

    void findDogsExtrema();

    std::vector<CImg<uint8_t>> getExtremas();

    void filterExtrema();

    void assignOrientation();

    std::vector<CImg<double>> getMagnitudes();

    std::vector<CImg<double>> getOrientations();
};


#endif //PANORAMA_FEATURE_EXTRACT_H
