//
// Created by Miguel Chan on 2018/11/29.
//

#ifndef PANORAMA_BLENDING_H
#define PANORAMA_BLENDING_H

#include "RANSAC.h"
#include <array>
#include <map>
#include "CImg.h"

using namespace cimg_library;

class blending {
public:
    typedef RANSAC::kPtPair kPtPair;
    const CImg<unsigned char> &src, &dst;
    const RANSAC::homography_mat &matAB, &matBA;
private:

public:
    blending(const CImg<unsigned char> &src, const CImg<unsigned char> &dst,
             const RANSAC::homography_mat &matAB, const RANSAC::homography_mat &matBA);

    CImg<unsigned char> run();

    int getWarpedWidth();

    int getWarpedHeight();

    double getWarpedX(double x, double y, const RANSAC::homography_mat& mat);

    double getWarpedY(double x, double y, const RANSAC::homography_mat& mat);

    double getMinX();

    double getMinY();

    void updateFeatureWithMat(std::map<std::array<float, 128>, VlSiftKeypoint> & feature);

    void updateFeatureWithOffset(std::map<std::array<float, 128>, VlSiftKeypoint> & feature);

    CImg<unsigned char> warpImage(int width, int height, int offset_x, int offset_y);

    CImg<unsigned char> moveImage(int width, int height, int offset_y, int offset_x);

    CImg<unsigned char> blend(CImg<unsigned char> & a, CImg<unsigned char> & b);

};


#endif //PANORAMA_BLENDING_H
