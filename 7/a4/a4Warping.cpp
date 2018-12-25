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
    if (intersects.size() != 4) {
        cerr << "Fail to warp!" << endl;
        return CImg<unsigned char>();
    }
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

//Returns the intersects in the order of top-left, top-right, bottom-right, bottom-left
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
    auto pt3 = intersects[points[3]];
    double k = (pt1.second - pt0.second) / (pt1.first - pt0.first);
    double b = pt1.second - (pt1.first * k);

    if ((k < 0 && b > 0 && pt0.first > pt1.first) || (k > 0 && pt0.second > pt3.second)) {
        swapVal(points[0], points[1]);
        swapVal(points[2], points[3]);
    }

    vector<pair<int, int>> res;
    for (auto pt : points) {
        res.push_back(intersects[pt]);
    }
    return res;
}

std::array<double, 8> a4Warping::getMatrix(const std::vector<std::pair<int, int>> &intersects) {
    const double x[4] = {static_cast<double>(intersects[0].first), static_cast<double>(intersects[1].first),
                         static_cast<double>(intersects[2].first), static_cast<double>(intersects[3].first)};
    const double y[4] = {static_cast<double>(intersects[0].second), static_cast<double>(intersects[1].second),
                         static_cast<double>(intersects[2].second), static_cast<double>(intersects[3].second)};
    CImg<double> A(4, 4, 1, 1, 0), bx(1, 4, 1, 1, 0), by(1, 4, 1, 1, 0);

    double width = sqrt(pow(intersects[0].first - intersects[1].first, 2) + pow(intersects[0].second - intersects[1].second, 2));
    double height = sqrt(2) * width;

    const double srcX[4] = {0, width, width, 0};
    const double srcY[4] = {0, 0, height, height};

    for (int i = 0; i < 4; i++) {
        A(0, i) = srcX[i];
        A(1, i) = srcY[i];
        A(2, i) = srcX[i] * srcY[i];
        A(3, i) = 1;
        bx(0, i) = x[i];
        by(0, i) = y[i];
    }

    CImg<double> mx = bx.get_solve(A);
    CImg<double> my = by.get_solve(A);

    return {mx(0, 0), mx(0, 1), mx(0, 2), mx(0, 3), my(0, 0), my(0, 1), my(0, 2), my(0, 3)};
}

double getMatX(const std::array<double, 8>& mat, double x, double y) {
    return mat[0] * x + mat[1] * y + mat[2] * x * y + mat[3];
}

double getMayY(const std::array<double, 8>& mat, double x, double y) {
    return mat[4] * x + mat[5] * y + mat[6] * x * y + mat[7];
}

CImg<unsigned char> a4Warping::transform(std::array<double, 8>& mat, const std::vector<std::pair<int, int>> &intersects) {
    double width = sqrt(pow(intersects[0].first - intersects[1].first, 2) + pow(intersects[0].second - intersects[1].second, 2));
    double height = sqrt(2) * width;
    CImg<unsigned char> res(width, height, 1, 3, 0);

    cimg_forXY(res, x, y) {
        double sX = getMatX(mat, x, y);
        double sY = getMayY(mat, x, y);
        for (int c = 0; c < 3; c++) {
            res(x, y, c) = utils::interpolate(src, sX, sY, c);
        }
    }

    return res;
}
