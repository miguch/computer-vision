//
// Created by Miguel Chan on 2018/12/26.
//

#include "OpenCV_utils.h"

using namespace cv;

cv::Mat OpenCV_utils::CImgToCvMat(const CImg<unsigned char> &img) {
    Mat result = Mat::zeros(img.height(), img.width(), CV_8UC1);
    cimg_forXY(img, x, y) {
        result.at<uchar>(y, x) = img(x, y);
    }

    return result;
}

cv::Mat OpenCV_utils::toTestImage(cv::Mat pSrc) {
    cv::Mat temp(pSrc.rows * pSrc.cols, 1, CV_8UC1), res;
    memcpy(temp.data, pSrc.data, sizeof(uchar) * pSrc.rows * pSrc.cols);
    temp.convertTo(res, CV_32FC1);
    return res / 255;
}

