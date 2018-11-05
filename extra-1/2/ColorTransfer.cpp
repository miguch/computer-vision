//
// Created by Miguel Chan on 2018/11/5.
//

#include "ColorTransfer.h"
#include <cmath>

//Convert RGB color space to lab
CImg<double> ColorTransfer::RGBtoLab(const CImg<unsigned char>& img) {
    //RGB to LMS
    CImg<double> lms(img.width(), img.height(), 1, 3, 0);
    cimg_forXY(img, x, y) {
        float r = img(x, y, 0), g = img(x, y, 1), b = img(x, y, 2);
        lms(x, y, 0) = 0.3811 * r + 0.5783 * g + 0.0402 * b; // l
        lms(x, y, 1) = 0.1967 * r + 0.7244 * g + 0.0782 * b; // m
        lms(x, y, 2) = 0.0241 * r + 0.1288 * g + 0.8444 * b; // s
    }
    //LMS to lab
    CImg<double> lab(img.width(), img.height(), 1, 3, 0);
    cimg_forXY(lms, x, y) {

        double L = lms(x, y, 0) == 0 ? 0 :log10(lms(x, y, 0)),
               M = lms(x, y, 1) == 0 ? 0: log10(lms(x, y, 1)),
               S = lms(x, y, 2) == 0 ? 0 : log10(lms(x, y, 2));
        lab(x, y, 0) = (L + M + S) / sqrt(3); //L
        lab(x, y, 1) = (L + M - 2 * S) / sqrt(6); //a
        lab(x, y, 2) = (L - M) / sqrt(2); //b
    }
    return lab;
}

ColorTransfer::ColorTransfer(string s, string t) : source(s.c_str()), target(t.c_str()) {}

//Convert Lab color space to RGB
CImg<unsigned char> ColorTransfer::LabtoRGB(const CImg<double> &img) {
    //lab to LMS
    CImg<double> lms(img.width(), img.height(), 1, 3, 0);
    cimg_forXY(img, x, y) {
            double l = img(x, y, 0) / sqrt(3), a = img(x, y, 1) / sqrt(6), b = img(x, y, 2) / sqrt(2);
            lms(x, y, 0) = l + a + b;
            lms(x, y, 1) = l + a - b;
            lms(x, y, 2) = l - a * 2;
        }
    //LMS to RGB
    CImg<unsigned char> rgb(img.width(), img.height(), 1, 3, 0);
    cimg_forXY(lms, x, y) {
            double L = pow(10, lms(x, y, 0)), M = pow(10, lms(x, y, 1)), S = pow(10, lms(x, y, 2));
            double r = 4.4679 * L - 3.5873 * M + 0.1193 * S; // l
            double g = -1.2186 * L + 2.3809 * M - 0.1624 * S; // m
            double b = 0.0497 * L - 0.2439 * M + 1.2045 * S; // s
            rgb(x, y, 0) = (unsigned char)(r > 255 ? 255 : (r < 0 ? 0 : r));
            rgb(x, y, 1) = (unsigned char)(g > 255 ? 255 : (g < 0 ? 0 : g));
            rgb(x, y, 2) = (unsigned char)(b > 255 ? 255 : (b < 0 ? 0 : b));
    }
    return rgb;
}


//Run the color transfer.
CImg<unsigned char> ColorTransfer::run() {
    auto sLab = RGBtoLab(source), tLab = RGBtoLab(target);
    auto sMean = getMean(sLab), tMean = getMean(tLab);
    auto sStdDev = getStdDev(sLab, sMean), tStdDev = getStdDev(tLab, tMean);
    //Subtract mean
    cimg_forXY(sLab, x, y) {
        for (int i = 0; i < 3; i++) {
            sLab(x, y, i) -= sMean[i];
            sLab(x, y, i) *= (tStdDev[i] / sStdDev[i]);
            sLab(x, y, i) += tMean[i];
        }
    }
    return LabtoRGB(sLab);
}

//Returns the mean of the 3 channels of the input img
array<double, 3> ColorTransfer::getMean(const CImg<double>& img) {
    array<double, 3> res = {0, 0, 0};
    cimg_forXY(img, x, y) {
        for (int i = 0; i < 3; i++) res[i] += img(x, y, i);
    }
    for (int i = 0; i < 3; i++) res[i] /= img.width() * img.height();
    return res;
}

//Returns the standard deviation of the 3 channels of the input img
array<double, 3> ColorTransfer::getStdDev(const CImg<double>& img, const array<double, 3>& mean) {
    array<double, 3> res = {0, 0, 0};
    cimg_forXY(img, x, y) {
        for (int i = 0; i < 3; i++) res[i] += pow(img(x, y, i) - mean[i], 2);
    }
    for (int i = 0; i < 3; i++) res[i] = sqrt(res[i] / (img.width() * img.height() - 1));
    return res;
}

CImg<unsigned char> ColorTransfer::getTarget() {
    return target;
}

CImg<unsigned char> ColorTransfer::getSource() {
    return source;
}
