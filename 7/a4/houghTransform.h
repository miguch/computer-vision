//
// Created by Miguel Chan on 2018/12/11.
//

#ifndef A4_HOUGHTRANSFORM_H
#define A4_HOUGHTRANSFORM_H

#include <vector>
#include "CImg.h"

using namespace cimg_library;

namespace houghTransform {
    std::vector<std::pair<int, int>> line_transform(const CImg<unsigned char> &src, double threshPercent);

    std::vector<std::pair<double, double>> polarToCartesian(const std::vector<std::pair<int, int>>& input);

    std::vector<std::pair<int, int>> findIntersect(const CImg<unsigned char> & src, const std::vector<std::pair<double, double>> &lines);

    void markEdges(const std::vector<std::pair<double, double>>& lines,
                   const std::vector<std::pair<int, int>>& intersects,
                             CImg<unsigned char> & origin);

    void drawCornorPoints(const std::vector<std::pair<int, int>> &points, CImg<unsigned char>& origin);
}

#endif //A4_HOUGHTRANSFORM_H
