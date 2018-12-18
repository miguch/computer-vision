//
// Created by Miguel Chan on 2018/12/17.
//

#include "testUtils.h"
#include <vector>
#include <stack>

using namespace std;
using namespace cv;

#include "mnistData.h"


double testBoost(std::array<adaboost, 10> &boosts) {
    auto data = mnistData::getInstance();
    int total = data->getTestSize();
    int corrects = 0;
    for (int i = 0; i < total; i++) {
        int result = -1;
        auto testImg = data->getTestImageData(i);
        for (int k = 0; k < 10; k++) {
            float predict = boosts[k].test(testImg);
            if (predict != 0) {
                result = k;
            }
        }
        if (result == data->getTestLabel(i)) ++corrects;
    }
    return (double) corrects / total;
}

Mat getTestImage(const char *filename) {
    Mat src = imread(filename);
    Mat adjusted;
    resize(src, adjusted, Size(28, 28));
    Mat thr, gray, res;
    cvtColor(adjusted, gray, COLOR_RGB2GRAY);
//    gray = ~gray;
    threshold(gray, thr, 130, 255, THRESH_BINARY_INV);
    imshow("test", thr);

    Mat temp(thr.rows * thr.cols, 1, CV_8UC1);
    memcpy(temp.data, thr.data, sizeof(uchar) * thr.rows * thr.cols);
    temp.convertTo(res, CV_32FC1);
    return res / 255;
}

Mat toTestImage(Mat pSrc) {
    Mat temp(pSrc.rows * pSrc.cols, 1, CV_8UC1), res;
    memcpy(temp.data, pSrc.data, sizeof(uchar) * pSrc.rows * pSrc.cols);
    temp.convertTo(res, CV_32FC1);
    return res / 255;
}

int testImg(Mat img, std::array<adaboost, 10> &boosts) {
    int result = -1;

    for (int k = 0; k < 10; k++) {
        float predict = boosts[k].test(img);
        if (predict != 0) {
//            if (result == -1) {
//                result = k;
//            } else {
//                result = -2;
//            }
            result = k;
        }
    }
    return result;
}

Mat getIntegralImage(Mat src) {
    Mat result = Mat::zeros(src.rows, src.cols, CV_32SC1);
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            if (x == 0) {
                if (y == 0) {
                    result.at<int>(y, x) = src.at<uchar>(y, x);
                } else {
                    result.at<int>(y, x) = result.at<int>(y - 1, x) + src.at<uchar>(y, x);
                }
            } else if (y == 0) {
                result.at<int>(y, x) = result.at<int>(y, x - 1) + src.at<uchar>(y, x);
            } else {
                result.at<int>(y, x) = src.at<uchar>(y, x) + result.at<int>(y - 1, x) +
                                       result.at<int>(y, x - 1) - result.at<int>(y - 1, x - 1);
            }
        }
    }
    return result;
}

int calculateWindow(const Mat &integral, int x, int y) {
    const int width = 28, height = 28;
    return integral.at<int>(y + height, x + width) + integral.at<int>(y, x) -
           integral.at<int>(y + height, x) - integral.at<int>(y, x + width);
}

CvRect growAtRegion(Mat &src, int x, int y, Mat &marked) {
    stack<Point> toVisit;
    toVisit.push(Point(x, y));
    int minX = INT_MAX, maxX = -1, minY = INT_MAX, maxY = -1;
    while (!toVisit.empty()) {
        Point curr = toVisit.top();
        toVisit.pop();
        if (curr.x < minX) minX = curr.x;
        if (curr.x > maxX) maxX = curr.x;
        if (curr.y < minY) minY = curr.y;
        if (curr.y > maxY) maxY = curr.y;
        for (int n = -3; n <= 3; n++) {
            for (int m = -3; m <= 3; m++) {
                if (m == 0 && n == 0) continue;
                int newX = curr.x + m;
                int newY = curr.y + n;
                if (newX < 0 || newX >= src.cols || newY < 0 || newY >= src.rows) {
                    continue;
                }
                if (src.at<uchar>(newY, newX) != 255) continue;
                if (marked.at<uchar>(newY, newX) != 0) continue;
                marked.at<uchar>(newY, newX) = 255;
                toVisit.push(Point(newX, newY));
            }
        }
    }
    return CvRect(minX - 3, minY - 3, maxX - minX + 6, maxY - minY + 6);
}

//Returns all rects of numbers
vector<CvRect> regionGrow(Mat &src) {
    int height = src.rows, width = src.cols;
    Mat marked = Mat::zeros(height, width, CV_8UC1);
    vector<CvRect> result;
    for (int y = 0; y < height - 20; y++) {
        for (int x = 0; x < width - 20; x++) {
            if (src.at<uchar>(y, x) == 255 && marked.at<uchar>(y, x) == 0) {
                auto rec = growAtRegion(src, x, y, marked);
                if (rec.height < 20 || rec.x < 0 || rec.y < 0 ||
                    rec.x + rec.width >= width || rec.y + rec.height >= height) {
                    continue;
                }
                result.push_back(rec);
            }
        }
    }
    return result;
}


void testA4(const char *filename, std::array<adaboost, 10> &boosts) {
    Mat a4 = imread(filename);
    resize(a4, a4, Size(a4.cols, a4.rows));
    Mat thr, gray, res;
    cvtColor(a4, gray, COLOR_RGB2GRAY);
    threshold(gray, thr, 150, 255, THRESH_BINARY_INV);
    Mat integral = getIntegralImage(thr);
    //Window size 28 * 28
    auto rects = regionGrow(thr);
    for (int i = 0; i < rects.size(); i++) {
        auto img = thr(rects[i]);
        resize(img, img, Size(28, 28));
        int result = testImg(toTestImage(img), boosts);
        rectangle(a4, rects[i], Scalar(0, 255, 0));
        putText(a4, to_string(result), Point(rects[i].x, rects[i].y + rects[i].height),
                cv::FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));
    }
    imwrite("digits.jpg", a4);
    imshow("", a4);
    waitKey(0);
}
