//
// Created by Miguel Chan on 2018/12/11.
//

#include "a4Warping.h"
#include "utils.h"
#include <array>
#include <limits>
#include <algorithm>

using namespace std;

a4Warping::a4Warping(const CImg<unsigned char> &src): src(src) {

}

//Run warping on source image with the input intersect points
//Assume that intersects.size() == 4
CImg<unsigned char> a4Warping::runWarping(const std::vector<std::pair<int, int>>& intersects) {
    auto ptList = getIntersectsList(intersects);
    auto mat = getMatrix(ptList);
    return transform(mat, ptList);

}

template <typename T>
void swapVal(T& a, T& b) {
    T temp = b;
    b = a;
    a = temp;
}

//Returns the intersects in the order of top-left, top-right, bottom-left, bottom-right
std::vector<std::pair<int, int>> a4Warping::getIntersectsList(const std::vector<std::pair<int, int>> &intersects) {
    //First find the point closest to (0,0) as one of the top points.
    //points is the sequence of point index in intersects
    //sequence in points: top-left, top-right, bottom-right, bottom-left
    vector<int> points = {0};
    int minDist = intersects[0].first * intersects[0].first +
                  intersects[0].second * intersects[0].second;
    for (int i = 1; i < intersects.size(); i++) {
        auto pt = intersects[i];
        int dist = pt.first * pt.first + pt.second * pt.second;
        if (dist < minDist) {
            points[0] = i;
            minDist = dist;
        }
    }

    //Find the points closest to top1 as the second top point.
    vector<pair<int, int>> ptDist;
    auto pt0 = intersects[points[0]];
    for (auto i = 0; i < intersects.size(); i++) {
        if (i == points[0]) continue;
        int diffX = pt0.first - intersects[i].first;
        int diffY = pt0.second - intersects[i].second;
        ptDist.push_back(make_pair(i, diffX * diffX + diffY * diffY));
    }
    sort(begin(ptDist), end(ptDist), [](pair<int, int> a, pair<int, int> b) {
        return a.second < b.second;
    });


    //top-right, bottom-right, bottom-left
    points.push_back(ptDist[0].first);
    points.push_back(ptDist[2].first);
    points.push_back(ptDist[1].first);


    //Calculate y = kx + b to see if we need to swap element.
    pt0 = intersects[points[0]];
    auto pt1 = intersects[points[1]];
    double k = (pt1.second - pt0.second) / (pt1.first - pt0.first);
    double b = pt1.second - (pt1.first * k);

    if ((k < 0 && b > 0 && pt0.first > pt1.first)) {
        swapVal(points[0], points[1]);
        swapVal(points[2], points[3]);
    }

    vector<pair<int, int>> res;
    for (auto pt : points) {
        res.push_back(intersects[pt]);
    }
    return res;
}

std::array<std::array<double, 3>, 3> a4Warping::getMatrix(const std::vector<std::pair<int, int>> &intersects) {
    const double x[4] = {static_cast<double>(intersects[0].first), static_cast<double>(intersects[1].first),
                         static_cast<double>(intersects[2].first), static_cast<double>(intersects[3].first)};
    const double y[4] = {static_cast<double>(intersects[0].second), static_cast<double>(intersects[1].second),
                         static_cast<double>(intersects[2].second), static_cast<double>(intersects[3].second)};
    double dx = x[0] - x[1] + x[2] - x[3];
    double dy = y[0] - y[1] + y[2] - y[3];
    if (dx == 0 && dy == 0) {
        return {{
            {x[1] - x[0], y[1] - y[0],      0},
            {x[2] - x[1], y[2] - y[1],      0},
            {x[0],        y[0],             1}
        }};
    }
    double dx1 = x[1] - x[2];
    double dx2 = x[2] - x[3];
    double dy1 = y[1] - y[2];
    double dy2 = y[2] - y[3];

    double denominator = dx1 * dy2 - dx2 * dy1;
    double a13 = (dx * dy2 - dx2 * dy) / denominator;
    double a23 = (dx1 * dy - dx * dy1) / denominator;
    return {{
        {x[1] - x[0] + a13 * x[1], y[1] - y[0] + a13 * y[1], a13},
        {x[3] - x[0] + a23 * x[3], y[3] - y[0] + a23* y[3],  a23},
        {x[0],                     y[0],                     1}
    }};
}

CImg<unsigned char> a4Warping::transform(std::array<std::array<double, 3>, 3>& mat, const std::vector<std::pair<int, int>> &intersects) {
    double width = sqrt(pow(intersects[0].first - intersects[1].first, 2) + pow(intersects[0].second - intersects[1].second, 2));
    double height = sqrt(2) * width;
    CImg<unsigned char> res(width, height, 1, 3, 0);

    cimg_forXY(res, x, y) {
        double rX = double(x) / width, rY = double(y) / height;
        double denominator = mat[0][2] * rX + mat[1][2] * rY + mat[2][2];
        double sX = (mat[0][0] * rX + mat[1][0] * rY + mat[2][0]) / denominator;
        double sY = (mat[0][1] * rX + mat[1][1] * rY + mat[2][1]) / denominator;
        for (int c = 0; c < 3; c++) {
            res(x, y, c) = utils::interpolate(src, sX, sY, c);
        }
    }

    return res;
}
