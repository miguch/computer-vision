//
// Created by Miguel Chan on 2018/12/10.
//

#include "kmeans.h"
#include <ctime>
#include "utils.h"
#include <cmath>
#include <limits>
#include <cstdlib>

using namespace std;

kmeans::kmeans(const CImg<unsigned char> &src) : origin(src) {
    srand(static_cast<unsigned int>(time(nullptr)));
    this->src = utils::RGBtoYCbCr(origin);
}

std::vector<pixelPoint> kmeans::getInitCentroids(int k) {
    vector<pixelPoint> res;
    //randomly selected the first centroid
    int initX = rand() % src.width(), initY = rand() % src.height();
    res.push_back({src(initX, initY, 0), src(initX, initY, 1), src(initX, initY, 2)});

    CImg<int> distances(src.width(), src.height(), 1, 1, 0);
    for (int i = 1; i < k; i++) {
        int sum = 0;
        cimg_forXY(src, x, y) {
            distances(x, y) = getMinDistance(point(x, y), res);
            sum += distances(x, y);
        }
        float f = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        sum *= f;
        cimg_forX(src, x) {
            bool toBreak = false;
            cimg_forY(src, y) {
                sum -= distances(x, y);
                if (sum <= 0) {
                    res.push_back({src(x, y, 0), src(x, y, 1), src(x, y, 2)});
                    toBreak = true;
                    break;
                }
            }
            if (toBreak) break;
        }
    }
    return res;
}

int kmeans::getMinDistance(point p, const std::vector<pixelPoint>& centroids) {
    int minDist = numeric_limits<int>::max();
    for (int i = 0; i < centroids.size(); i++) {
        int dist = getDistance(p, centroids[i]);
        if (dist < minDist) {
            minDist = dist;
        }
    }
    return minDist;
}

//Returns the square of the Euclidean distance bewteen the two points
int kmeans::getDistance(point a, point b) {
    int dist = 0;
    for (int i = 0; i < src.spectrum(); i++) {
        int d = src(a.first, a.second, i) - src(b.first, b.second, i);
        dist += d * d;
    }
    return dist;
}

int kmeans::getDistance(point a, const pixelPoint& b) {
    int dist = 0;
    for (int i = 0; i < 3; i++) {
        int d = src(a.first, a.second, i) - b[i];
        dist += d * d;
    }
    return dist;
}


vector<int> kmeans::getKmeans(vector<pixelPoint>& centroids) {
    int k = centroids.size();
    vector<int> res(src.width() * src.height(), k + 1);
    //Whether the centroids need to be recalculated.
    bool reCalc = true;
    while (reCalc) {
        reCalc = false;
        cimg_forXY(src, x, y) {
            int minDist = numeric_limits<int>::max();
            int minIndex = 0;
            for (int i = 0; i < k; i++) {
                int dist = getDistance({x, y}, centroids[i]);
                if (dist < minDist) {
                    minDist = dist;
                    minIndex = i;
                }
            }
            int index = pointToIndex({x, y});
            if (minIndex != res[index]) {
                reCalc = true;
                res[index] = minIndex;
            }
        }
        for (int i = 0; i < k; i++) {
            array<int, 3> sum = {0, 0, 0};
            int count = 0;
            cimg_forXY(src, x, y) {
                if (i == res[pointToIndex({x, y})]) {
                    count += 1;
                    for (int c = 0; c < 3; c++) {
                        sum[c] += src(x, y, c);
                    }
                }
            }
            if (count == 0) continue;
            for (int c = 0; c < 3; c++) {
                centroids[i][c] = static_cast<unsigned char>(sum[c] / count);
            }
        }
    }
    return res;
}


point kmeans::indexToPoint(int index) {
    return {index % src.width(), index / src.width()};
}

int kmeans::pointToIndex(point p) {
    return p.first + p.second * src.width();
}

CImg<unsigned char>
kmeans::drawClusterImage(std::vector<int> means, const std::vector<pixelPoint> &centroids) {
    CImg<unsigned char> result(src.width(), src.height(), 1, 3, 0);
    cimg_forXY(src, x, y) {
        int index = means[pointToIndex({x, y})];
        auto pix = utils::YCbCrtoRGBPix(centroids[index]);
        for (int c = 0; c < 3; c++) {
            result(x, y, c) = pix[c];
        }
    }
    return result;
}

CImg<unsigned char> kmeans::runKmeans(int k, bool thresholding) {
    auto centroids = getInitCentroids(k);
    auto means = getKmeans(centroids);
    if (thresholding) {
        return drawBinaryImage(means, centroids);
    } else {
        return drawClusterImage(means, centroids);
    }
}

//Require k == 2
CImg<unsigned char> kmeans::drawBinaryImage(std::vector<int> means, const std::vector<pixelPoint> &centroids) {
    CImg<unsigned char> result(src.width(), src.height(), 1, 1, 0);
    auto pix0 = utils::YCbCrtoRGBPix(centroids[0]);
    auto pix1 = utils::YCbCrtoRGBPix(centroids[1]);
    unsigned char g0 = utils::getGreyPixel(pix0[0], pix0[1], pix0[2]);
    unsigned char g1 = utils::getGreyPixel(pix1[0], pix1[1], pix1[2]);
    unsigned char pix[2] = {static_cast<unsigned char>(g0 > g1 ? 255 : 0),
                            static_cast<unsigned char>(g0 < g1 ? 255 : 0)};
    cimg_forXY(src, x, y) {
            int index = means[pointToIndex({x, y})];
            result(x, y) = pix[index];
        }
    return result;
}





