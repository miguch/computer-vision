//
// Created by Miguel Chan on 2018/12/25.
//

#ifndef RECOGNITION_SEGMENTATION_H
#define RECOGNITION_SEGMENTATION_H

#include "src/CImg.h"
#include <vector>

using namespace cimg_library;

class segmentation {
private:
    const CImg<unsigned char> &src;

public:
    explicit segmentation(const CImg<unsigned char> &src);

    static std::vector<int> getRowsIntensity(const CImg<unsigned char>& img);
    static std::vector<int> getColsIntensity(const CImg<unsigned char>& img, int y1, int y2);
    std::vector<std::vector<CImg<unsigned char>>> runSegmentation();

    static std::vector<std::pair<int, int>> getRowSegments(const CImg<unsigned char>& img, const std::vector<int> &rows);
    static std::vector<std::pair<int, int>> getColSegments(const CImg<unsigned char>& img, const std::vector<int> &cols);

    static std::vector<CImg<unsigned char>> regionSegment(const std::vector<CImg<unsigned char>>& originSegs);
};


#endif //RECOGNITION_SEGMENTATION_H
