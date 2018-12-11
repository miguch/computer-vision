//
// Created by Miguel Chan on 2018/12/11.
//

#include "houghTransform.h"
#include <iostream>

using namespace std;

namespace houghTransform {

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

    const int houghSpaceSlice = 360;
    const double interval = 2 * M_PI / houghSpaceSlice;


    vector<vector<int>> getHoughSpace(const CImg<unsigned char> &src) {
        //Y-axis is 360 slices of [0, 2PI]
        const int maxLength = (int) sqrt(pow(src.width(), 2) + pow(src.height(), 2)) + 1;

        vector<vector<int>> houghSpace = vector<vector<int>>(houghSpaceSlice, vector<int>(maxLength, 0));

        cimg_forX(src, x) {
            cimg_forY(src, y) {
                if (src(x, y) != 255) continue;
                double theta0 = atan2(y, x);
                double r0 = sqrt(pow(x, 2) + pow(y, 2));
                for (int i = 0; i < houghSpaceSlice; i++) {
                    int row = i;
                    double theta = i * interval;
                    double r = r0 * cos(theta0 - theta);
                    if (r < 0) {
                        r = -r;
                        row = (i + houghSpaceSlice / 2) % houghSpaceSlice;
                    }
                    houghSpace[row][(int) r]++;
                }
            }
        }

        return houghSpace;
    }

    //Returns r-θ pairs
    std::vector<std::pair<int, int>> line_transform(const CImg<unsigned char> &src, double threshPercent) {
        /**********Part1: Get hough space result ******************/
        auto houghSpace = getHoughSpace(src);
        const int maxLength = (int) sqrt(pow(src.width(), 2) + pow(src.height(), 2)) + 1;

        /***********Part 2: Find maximum hough value to generate threshold********/
        int maxHough = 0;
        for (int y = 0; y < houghSpaceSlice; y++) {
            for (int x = 0; x < maxLength; x++) {
                if (houghSpace[y][x] > maxHough) {
                    maxHough = houghSpace[y][x];
                }
            }
        }

        /**********Part 3: Vote for lines********************/
        int thresh = int((double) maxHough * threshPercent);
        //θ-r pairs
        vector<pair<int, int>> lines;
        for (int y = 0; y < houghSpaceSlice; y++) {
            for (int x = 0; x < maxLength; x++) {
                if (houghSpace[y][x] < thresh) continue;
                bool addNewLine = true;
                for (auto &line : lines) {
                    //If the two line is close enough, see them as one line
                    if (abs(line.second - x) < maxLength / 20 &&
                        (abs(line.first - y) < 20 || abs(line.first - y) > houghSpaceSlice - 20 ||
                            abs(abs(line.first - y) - 180) < 10)) {
                        addNewLine = false;
                        if (houghSpace[y][x] > houghSpace[line.first][line.second]) {
                            //Current line is more likely since it has more votes.
                            line.first = y;
                            line.second = x;
                        }
                    }
                }
                if (addNewLine) {
                    lines.push_back(make_pair(y, x));
                }
            }
        }

        return lines;
    }

    std::vector<std::pair<double, double>> polarToCartesian(const vector<std::pair<int, int>> &input) {
        vector<pair<double, double>> res;
        for (auto & line : input) {
            double angle = line.first * interval;
            if (angle == 0) angle += (double)1 / houghSpaceSlice;
            double k = -cos(angle) / sin(angle);
            double b = line.second / sin(angle);
            res.push_back(make_pair(k, b));
        }
        return res;
    }

    std::vector<std::pair<int, int>> findIntersect(const CImg<unsigned char> & src, const std::vector<std::pair<double, double>> &lines) {
        vector<pair<int, int>> res;
        for (int i = 0; i < lines.size(); i++) {
            for (int k = i + 1; k < lines.size(); k++) {
                double x = (lines[k].second - lines[i].second) / (lines[i].first - lines[k].first);
                double y = x * lines[k].first + lines[k].second;
                if (x >= 0 && x < src.width() && y >= 0 && y < src.height()) {
                    res.emplace_back((int) x, (int) y);
                }
            }
        }
        return res;
    }



    void
    markEdges(const std::vector<std::pair<double, double>> &lines, const std::vector<std::pair<int, int>> &intersects,
              CImg<unsigned char> &origin) {
        int max_x = origin.width(), max_y = origin.height();
        const unsigned char colorRed[] = {255, 0, 0};
        for (int i = 0; i < intersects.size(); i++) {
            for (int k = 0; k < intersects.size(); k++) {
                if (i == k) continue;
                for (auto &line : lines) {
                    double y0 = line.first * intersects[i].first + line.second;
                    double y1 = line.first * intersects[k].first + line.second;
                    if (abs(y0 - intersects[i].second) < max_y / 10 &&
                        abs(y1 - intersects[k].second) < max_y / 10) {
                        origin.draw_line(intersects[i].first, intersects[i].second,
                                         intersects[k].first, intersects[k].second, colorRed);
                    }
                }
            }
        }
    }

    void drawCornorPoints(const std::vector<std::pair<int, int>> &points, CImg<unsigned char> &origin) {
        const unsigned char colorGreen[] = {0, 255, 0};
        for (auto point : points) {
            origin.draw_circle(point.first, point.second, 50, colorGreen);
        }

    }

}