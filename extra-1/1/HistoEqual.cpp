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
    for (int i = 0; i < 3; i++) {
        equalizeImageChannel(origin, i, nb_level);
    }
    return origin;
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


