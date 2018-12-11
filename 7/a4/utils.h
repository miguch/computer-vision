//
// Created by Miguel Chan on 2018/11/27.
//

#ifndef A4_UTILS_H
#define A4_UTILS_H

#include "CImg.h"
#include <cstdint>
#include <array>

using namespace cimg_library;

namespace utils {

    unsigned char interpolate(const CImg<unsigned char> &src, double x, double y, int channel);
    CImg<unsigned char> toGreyScale(const CImg<unsigned char>& src);

    unsigned char getGreyPixel(unsigned char r, unsigned char g, unsigned char b);

    CImg<double> RGBtoLab(const CImg<unsigned char> src);
    CImg<unsigned char> LabtoRGB(const CImg<double> src);

    std::array<double, 3> RGBtoLabPix(unsigned char r, unsigned char g, unsigned char b);
    std::array<unsigned char, 3> LabtoRGBPix(double l, double a, double b);

    CImg<unsigned char> RGBtoYCbCr(const CImg<unsigned char>& img);
    CImg<unsigned char> YCbCrtoRGB(const CImg<unsigned char> &img);

    typedef std::array<unsigned char, 3> point;
    point RGBtoYCbCrPix(const point& pix);
    point YCbCrtoRGBPix(const point& pix);

    CImg<unsigned char> getOutline(const CImg<unsigned char> &src);
};


#endif //PANORAMA_UTILS_H
