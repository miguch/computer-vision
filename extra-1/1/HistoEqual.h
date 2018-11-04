//
// Created by Miguel Chan on 2018/11/4.
//

#ifndef INC_1_HISTOEQUAL_H
#define INC_1_HISTOEQUAL_H

#include <string>
#include "CImg.h"

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
    CImg<unsigned char> runWithGreyScale(int nb_level);
    CImg<unsigned char> runWithRGB(int nb_level);
    CImg<unsigned char> getRGBHistogram();
    CImg<unsigned char> getGreyHistogram();

};


#endif //INC_1_HISTOEQUAL_H
