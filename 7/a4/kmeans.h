//
// Created by Miguel Chan on 2018/12/10.
//

#ifndef A4_KMEANS_H
#define A4_KMEANS_H

#include "CImg.h"
#include <iostream>
#include <vector>
#include <array>

using namespace cimg_library;

typedef std::pair<int, int> point;
//(r, g, b)
typedef std::array<unsigned char, 3> pixelPoint;

class kmeans {
private:
    const CImg<unsigned char> &origin;
    CImg<unsigned char> src;

public:
    explicit kmeans(const CImg<unsigned char> &src);
    std::vector<pixelPoint> getInitCentroids(int k);
    int getMinDistance(point p, const std::vector<pixelPoint>& centroids);
    int getDistance(point a, point b);
    int getDistance(point a, const pixelPoint& b);
    std::vector<int> getKmeans(std::vector<pixelPoint>& centroids);
    inline point indexToPoint(int index);
    inline int pointToIndex(point p);
    CImg<unsigned char> drawClusterImage(std::vector<int> means, const std::vector<pixelPoint>&centroids);
    CImg<unsigned char> runKmeans(int k, bool thresholding);
    CImg<unsigned char> drawBinaryImage(std::vector<int> means, const std::vector<pixelPoint>&centroids);

};


#endif //A4_KMEANS_H
