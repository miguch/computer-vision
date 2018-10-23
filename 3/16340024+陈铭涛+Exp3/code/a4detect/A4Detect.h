//
// Created by Miguel Chan on 2018/10/18.
//

#ifndef INC_3_1_A4DETECT_H
#define INC_3_1_A4DETECT_H
#include <string>
#include <vector>
#include "canny.h"

using namespace std;

class A4Detect {
private:
    string _filename;
    CImg<unsigned char> origin;
    static vector<vector<int>> getHoughSpace(const CImg<unsigned char>& origin);
    const static int houghSpaceSlice;
    const static double interval;
    static void saveHoughSpace(const vector<vector<int>>& hough);
    static void drawLineFromCartesian(vector<pair<double, double>> lines, CImg<unsigned char>& origin);
    static void drawLineFromPolar(vector<pair<int, int>> lines, CImg<unsigned char>& origin);
    vector<pair<int, int>> findIntersect(const vector<pair<double, double>>& lines);
    static void drawCornorPoints(const vector<pair<int, int>>& points, CImg<unsigned char>& origin);
    static void markEdges(const vector<pair<double, double>>& lines, const vector<pair<int, int>>& intersect, CImg<unsigned char> & origin);
    const float _thresh;
    const int cannyThresh;
    const bool skipNMS;

public:
    A4Detect(string filename, float thresh, int cannyThr, bool skipNMS);
    CImg<unsigned char> getCanny(bool skipNMS, int thresh1, int thresh2, int scale, float sigma, bool connect, bool deleteShort);
    CImg<unsigned char> runHough();
    static vector<pair<int, int>> houghTransform(const CImg<unsigned char>& grad, float threshPercent);
    static vector<pair<double, double>> polarToCartesian(const vector<pair<int, int>>& input);


};


#endif //INC_3_1_A4DETECT_H
