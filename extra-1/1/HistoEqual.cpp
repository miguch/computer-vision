//
// Created by Miguel Chan on 2018/11/4.
//

#include <vector>
#include <iostream>
#include "HistoEqual.h"


HistoEqual::HistoEqual(string filename) : filename(filename), origin(filename.c_str()) {
    toGreyScaleImage();
}

void HistoEqual::toGreyScaleImage() {
    greyScale = CImg<unsigned char>(origin.width(), origin.height());
    cimg_forXY(origin, x, y) {
        auto r = origin(x, y, 0), g = origin(x, y, 1), b = origin(x, y, 2);
        greyScale(x, y) = 0.299 * r + 0.587 * g + 0.114 * b;
    }
}

//nb_level:	Number of desired histogram levels.
//Returns the Histogram equalized RGB image
CImg<unsigned char> HistoEqual::runWithRGB(int nb_level) {
    CImg<unsigned char> copy(origin);
    for (int i = 0; i < 3; i++) {
        equalizeImageChannel(copy, i, nb_level);
    }
    return copy;
}

//nb_level:	Number of desired histogram levels.
//Returns the Histogram equalized grey scale image
CImg<unsigned char> HistoEqual::runWithGreyScale(int nb_level) {
    equalizeImageChannel(greyScale, 0, nb_level);
    return greyScale;
}

//Returns the histogram of the grey scale image using CImg histogram implementation
CImg<unsigned char> HistoEqual::getGreyHistogram() {
    return greyScale.get_histogram(256, 0, 255);
}

//Returns the histogram of the original RGB image using CImg histogram implementation
CImg<unsigned char> HistoEqual::getRGBHistogram() {
    return origin.get_histogram(256, 0, 255);
}

//Returns the cumulative distribution function of a specific channel of a image
vector<float> HistoEqual::getChannelCDF(const CImg<unsigned char>& img, int channel) {
    vector<float> hist(256, 0);
    cimg_forXY(img, x, y) {
            hist[img(x, y, channel)] += 1;
    }
    for (int i = 0; i < 256; i++) {
        hist[i] /= img.height() * img.width();
        if (i >= 1) {
            hist[i] += hist[i-1];
        }
    }
    return hist;
}

void HistoEqual::equalizeImageChannel(CImg<unsigned char> &input, int channel, int nb_level) {
    auto cdf = getChannelCDF(input, channel);
    for (auto & val : cdf) {
        val = val * 255 + 0.5;
    }
    cimg_forXY(input, x, y) {
            int pos = input(x, y, channel);
            pos = int(pos * ((double)nb_level / 256) + 1) * (256 / nb_level) - 1;
            input(x, y, channel) = (unsigned char)(cdf[pos]);
    }
}


//Convert RGB color space to YCbCr color space
CImg<unsigned char> HistoEqual::RGBtoYCbCr(const CImg<unsigned char>& img) {
    CImg<unsigned char> ycbcr(img.width(), img.height(), 1, 3, 0);
    cimg_forXY(img, x, y) {
        auto r = img(x, y, 0), g = img(x, y, 1), b = img(x, y, 2);
        int Y = 0.299 * r + 0.587 * g + 0.114 * b; // Y
        int Cr = 128 - 0.168736 * r - 0.331264 * g + 0.5 * b; //CR
        int Cb = 128 + 0.5 * r - 0.418688 * g - 0.081312 * b; //CB
        ycbcr(x, y, 0) = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
        ycbcr(x, y, 1) = (unsigned char)(Cr > 255 ? 255 : (Cr < 0 ? 0 : Cr));
        ycbcr(x, y, 2) = (unsigned char)(Cb > 255 ? 255 : (Cb < 0 ? 0 : Cb));
    }
    return ycbcr;
}

//Convert YCbCr color space to RGB
CImg<unsigned char> HistoEqual::YCbCrtoRGB(const CImg<unsigned char> &img) {
    //lab to LMS
    CImg<double> rgb(img.width(), img.height(), 1, 3, 0);
    cimg_forXY(img, x, y) {
         auto Y = img(x, y, 0), cB = img(x, y, 1), cR = img(x, y, 2);
         int r = Y + 1.402 * (cR - 128);
         int g = Y - 0.344136 * (cB - 128) - 0.714136 * (cR - 128);
         int b = Y + 1.772 * (cB - 128);
         rgb(x, y, 0) = (unsigned char)(r > 255 ? 255 : (r < 0 ? 0 : r));
         rgb(x, y, 1) = (unsigned char)(g > 255 ? 255 : (g < 0 ? 0 : g));
         rgb(x, y, 2) = (unsigned char)(b > 255 ? 255 : (b < 0 ? 0 : b));
        }

    return rgb;
}

CImg<unsigned char> HistoEqual::runWithYCbCr(int nb_level) {
    auto ycbcr = RGBtoYCbCr(origin);

    //Only equalize the Y channel
    equalizeImageChannel(ycbcr, 0, nb_level);

    return YCbCrtoRGB(ycbcr);
}

CImg<unsigned char> HistoEqual::getOriginImage() {
    return origin;
}

CImg<unsigned char> HistoEqual::getGreyScale() {
    return greyScale;
}

