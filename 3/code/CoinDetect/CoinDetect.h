//
// Created by Miguel Chan on 2018/10/21.
//

#ifndef COINDETECT_COINDETECT_H
#define COINDETECT_COINDETECT_H

#include "canny.h"
#include <vector>
#include <array>

class CoinDetect {
private:
    const int cannyThresh;
    const string _filename;
    const double houghThreshPercent;
    CImg<unsigned char> origin;
    static const int houghSpaceSlice;
    static const double interval;
    const int low_r, high_r;
    //Circle is defined by (x, y, r)
    typedef std::array<int, 3> circle;
    vector<circle> getHoughCircles(const CImg<unsigned char> &edges);
    static void drawCircle(const vector<circle>& circles, CImg<unsigned char> & canvas);


public:
    CoinDetect(double houghThresh, int cannyThresh, string filename, int lowR, int highR);
    CImg<unsigned char> runHough();

};


#endif //COINDETECT_COINDETECT_H
