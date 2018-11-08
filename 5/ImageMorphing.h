//
// Created by Miguel Chan on 2018/11/7.
//

#ifndef INC_5_IMAGEMORPHING_H
#define INC_5_IMAGEMORPHING_H

#include "CImg.h"
#include <string>
#include <vector>

using namespace std;
using namespace cimg_library;

class ImageMorphing {
public:
    typedef pair<int, int> point;
private:

    CImg<unsigned char> source;
    CImg<unsigned char> destination;
    static CImg<unsigned char> warp(const CImg<unsigned char> & src, vector<pair<point, point>> oldPt, vector<pair<point, point>> newPt);
    static vector<pair<point, point>> makeLines(const vector<point>& points);

public:
    ImageMorphing(string s, string d);
    pair<CImg<unsigned char>,CImg<unsigned char>> morph(const vector<pair<point, point>>& points, unsigned intermediates);

    vector<pair<point, point>> selectPoints(const vector<pair<point, point>>& existing = vector<pair<point, point>>());
    static vector<pair<point, point>> readPointsFromFile(string filename);
    static void savePointToFile(const vector<pair<point, point>>& points, string filename);
};


#endif //INC_5_IMAGEMORPHING_H
