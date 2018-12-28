//
// Created by Miguel Chan on 2018/11/4.
//

#ifndef INC_1_HISTOEQUAL_H
#define INC_1_HISTOEQUAL_H

#include <string>
#include "src/CImg.h"
#include <vector>

using namespace std;
using namespace cimg_library;

class HistoEqual {

private:
    string filename;
    void toGreyScaleImage();
    CImg<unsigned char> origin;
    CImg<unsigned char> greyScale;
    static vector<float> getChannelCDF(const CImg<unsigned char>& img, int channel);
    static void equalizeImageChannel(CImg<unsigned char>& input, int channel, int nb_level);
public:
    explicit HistoEqual(string filename);

    explicit HistoEqual(const CImg<unsigned char>& src);
    CImg<unsigned char> runWithGreyScale(int nb_level);
    CImg<unsigned char> runWithRGB(int nb_level);
    CImg<unsigned char> runWithYCbCr(int nb_level);
    CImg<unsigned char> getRGBHistogram();
    CImg<unsigned char> getGreyHistogram();
    CImg<unsigned char> getOriginImage();
    CImg<unsigned char> getGreyScale();

    static CImg<unsigned char> RGBtoYCbCr(const CImg<unsigned char>& img);
    static CImg<unsigned char> YCbCrtoRGB(const CImg<unsigned char>& img);

};


#endif //INC_1_HISTOEQUAL_H
