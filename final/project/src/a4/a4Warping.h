//
// Created by Miguel Chan on 2018/12/11.
//

#ifndef A4_A4WARPING_H
#define A4_A4WARPING_H

#include "../CImg.h"
#include <vector>
#include <iostream>
#include <array>

using namespace cimg_library;

class a4Warping {

private:
    const CImg<unsigned char> & src;

public:
    explicit a4Warping(const CImg<unsigned char> & src);

    CImg<unsigned char> runWarping(const std::vector<std::pair<int, int>>& intersects);

    std::vector<std::pair<int, int>> getIntersectsList(const std::vector<std::pair<int, int>>& intersects);

    std::array<double, 8> getMatrix(const std::vector<std::pair<int, int>>& intersects);

    CImg<unsigned char> transform(std::array<double, 8>& mat, const std::vector<std::pair<int, int>> &intersects);


};


#endif //A4_A4WARPING_H
