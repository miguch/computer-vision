//
// Created by Miguel Chan on 2018/10/18.
//

#include <fstream>
#include <vector>
#include <thread>
#include "A4Detect.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif


A4Detect::A4Detect(string filename, float thresh, int cannyThr, bool skipNMS) : _filename(filename),
                                      origin(filename.c_str()),
                                      _thresh(thresh),
                                        cannyThresh(cannyThr),
                                        skipNMS(skipNMS){}

const int A4Detect::houghSpaceSlice = 360;
const double A4Detect::interval = 2 * M_PI / A4Detect::houghSpaceSlice;


CImg<unsigned char>
A4Detect::getCanny(bool skipNMS, int thresh1, int thresh2, int scale, float sigma, bool connect, bool deleteShort) {
    Canny c(_filename, skipNMS, thresh1, thresh2, scale, sigma);
    CImg<unsigned char> res = c.runCanny();
    if (connect) res = c.connectEdges(res);
    if (deleteShort) res = c.deleteShortEdges(res);
    return res;
}


CImg<unsigned char> A4Detect::runHough() {
    Canny c(_filename, skipNMS, cannyThresh, cannyThresh, 4, 2);
    auto edges = c.runCanny();
    edges.save("cannyEdges.bmp");
    auto polarLines = houghTransform(edges, _thresh);
    auto carteLines = polarToCartesian(polarLines);
    auto intersects = findIntersect(carteLines);
    markEdges(carteLines, intersects, origin);
    drawCornorPoints(intersects, origin);
    origin.save("corners.bmp");

    return origin;
}

const int dirs[][2] = {{0,  -1},
                       {1,  -1},
                       {1,  0},
                       {1,  1},
                       {0,  1},
                       {-1, 1},
                       {-1, 0},
                       {-1, -1}};


/*****************Returns the lines generated from the votes as θ-r pairs********/
vector<pair<int, int>> A4Detect::houghTransform(const CImg<unsigned char>& grad, float threshPercent) {
    /**********Part1: Get hough space result ******************/
    auto houghSpace = getHoughSpace(grad);
    const int maxLength = (int) sqrt(pow(grad.width(), 2) + pow(grad.height(), 2)) + 1;

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
    int thresh = int((double)maxHough * threshPercent);
    //θ-r pairs
    vector<pair<int, int>> lines;
    printf("Using %d as threshold.\n", thresh);
    cout << "MaxLength: " << maxLength << endl;
    for (int y = 0; y < houghSpaceSlice; y++) {
        for (int x = 0; x < maxLength; x++) {
            if (houghSpace[y][x] < thresh) continue;
            bool addNewLine = true;
            for (auto &line : lines) {
                //If the two line is close enough, see them as one line
                if (abs(line.second - x) < maxLength / 40 && (abs(line.first - y) < 10 || abs(line.first - y) > houghSpaceSlice - 10)) {
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


/**********************Returns the hough space generated from gradients **********/
vector<vector<int>> A4Detect::getHoughSpace(const CImg<unsigned char> &grad) {
    //Y-axis is 360 slices of [0, 2PI]
    const int maxLength = (int) sqrt(pow(grad.width(), 2) + pow(grad.height(), 2)) + 1;

    //Use vector<vector<int>> instead of CImg<int> to avoid copy at return (C++11).
    vector<vector<int>> houghSpace = vector<vector<int>>(houghSpaceSlice, vector<int>(maxLength, 0));

    cimg_forX(grad, x) {
        cimg_forY(grad, y) {
            if (grad(x, y) != 255) continue;
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

    saveHoughSpace(houghSpace);
    return houghSpace;
}

/********************Save the hough space to a bmp file******************/
void A4Detect::saveHoughSpace(const vector<vector<int>> &hough) {
    CImg<int> img(hough[0].size(), hough.size(), 1, 1, 0);
    for (int i = 0; i < hough.size(); i++) {
        for (int j = 0; j < hough[i].size(); j++) {
            img(j, i) = hough[i][j];
        }
    }
    //img.save("hough.bmp");
}

/******************Draw θ-r pairs lines on the input CImg object************/
void A4Detect::drawLineFromPolar(vector<pair<int, int>> lines, CImg<unsigned char>& origin) {
    for (auto &line : lines) {
        cout << line.first << " " << line.second << endl;
        double angle = line.first * interval;
        double k = -cos(angle) / sin(angle);
        double b = line.second / sin(angle);
        cout << "k=" << k << " b=" << b << endl;
        int startX = -b / k, startY = b,
                endX = (origin.height() - b) / k, endY = origin.width() * k + b;
        const unsigned char colorBlue[] = {0,0,255};
        if (k > 1) {
            origin.draw_line(startX, 0, endX, origin.height(), colorBlue);
        } else {
            origin.draw_line(0, startY, origin.width(), endY, colorBlue);
        }
    }
}

/*****Returns lines as k-b pairs***************/
vector<pair<double, double>> A4Detect::polarToCartesian(const vector<pair<int, int>>& input) {
    vector<pair<double, double>> res;
    for (auto & line : input) {
        double angle = line.first * interval;
        if (angle == 0) angle += (double)1 / houghSpaceSlice;
        double k = -cos(angle) / sin(angle);
        double b = line.second / sin(angle);
        cout << "y=" << k << "x" << (b > 0 ? "+":"") << b << endl;
        res.push_back(make_pair(k, b));
    }
    return res;
}

/************Find all intersect points in all lines and returns as (x,y) pairs******/
vector<pair<int, int>> A4Detect::findIntersect(const vector<pair<double, double>> &lines) {
    vector<pair<int, int>> res;
    for (int i = 0; i < lines.size(); i++) {
        for (int k = i + 1; k < lines.size(); k++) {
            double x = (lines[k].second - lines[i].second) / (lines[i].first - lines[k].first);
            double y = x * lines[k].first + lines[k].second;
            if (x >= 0 && x < origin.width() && y >= 0 && y < origin.height()) {
                res.emplace_back((int) x, (int) y);
                cout << "(" << x << "," << y << ")" << endl;
            }
        }
    }
    return res;
}

/****************Given a k-b pairs line and draw all lines on the input image **********/
void A4Detect::drawLineFromCartesian(vector<pair<double, double>> lines, CImg<unsigned char> &origin) {
    for (auto &line : lines) {
        double k = line.first;
        double b = line.second;
        int startX = -b / k, startY = b,
                endX = (origin.height() - b) / k, endY = origin.width() * k + b;
        const unsigned char colorBlue[] = {0,0,255};
        if (k > 1) {
            origin.draw_line(startX, 0, endX, origin.height(), colorBlue);
        } else {
            origin.draw_line(0, startY, origin.width(), endY, colorBlue);
        }
    }
}

/********************Draw all the points on the input image****************************/
void A4Detect::drawCornorPoints(const vector<pair<int, int>> &points, CImg<unsigned char>& origin) {
    const unsigned char colorGreen[] = {0, 255, 0};
    for (auto point : points) {
        origin.draw_circle(point.first, point.second, 50, colorGreen);
    }
}


/**Mark the parts of lines that are
 * between two corner points
 * on the original image.
 */
void A4Detect::markEdges(const vector<pair<double, double>>& lines, const vector<pair<int, int>>& intersects,
                         CImg<unsigned char> & origin) {
    int max_x = origin.width(), max_y = origin.height();
    const unsigned char colorRed[] = {255, 0, 0};
    for (int i = 0; i < intersects.size(); i++) {
        for (int k = 0; k < intersects.size(); k++) {
            for (auto &line : lines) {
                double y0 = line.first * intersects[i].first + line.second;
                double y1 = line.first * intersects[k].first + line.second;
                if (abs(y0 - intersects[i].second) < max_y / 40 &&
                    abs(y1 - intersects[k].second) < max_y / 40) {
                    origin.draw_line(intersects[i].first, intersects[i].second,
                                    intersects[k].first, intersects[k].second, colorRed);
                }
            }
        }
    }
}


