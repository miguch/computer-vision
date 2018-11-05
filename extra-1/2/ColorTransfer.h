//
// Created by Miguel Chan on 2018/11/5.
//

#ifndef INC_2_COLORTRANSFER_H
#define INC_2_COLORTRANSFER_H
#include <string>
#include <array>
#include "CImg.h"

using namespace cimg_library;
using namespace std;

class ColorTransfer {
private:
    CImg<unsigned char> source, target;
    static array<double, 3> getMean(const CImg<double>& img);
    static array<double, 3> getStdDev(const CImg<double>& img, const array<double, 3>& mean);

public:
    ColorTransfer(string s, string t);
    static CImg<double> RGBtoLab(const CImg<unsigned char>& img);
    static CImg<unsigned char> LabtoRGB(const CImg<double>& img);
    CImg<unsigned char> run();
};


#endif //INC_2_COLORTRANSFER_H
