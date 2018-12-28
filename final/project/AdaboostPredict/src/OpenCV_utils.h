//
// Created by Miguel Chan on 2018/12/26.
//

#ifndef RECOGNITION_OPENCV_UTILS_H
#define RECOGNITION_OPENCV_UTILS_H

//Include first Avoid X11 macro conflict
#include <opencv2/opencv.hpp>
#include "CImg.h"

using namespace cimg_library;

namespace OpenCV_utils {
    cv::Mat CImgToCvMat(const CImg<unsigned char> &img);

    cv::Mat toTestImage(cv::Mat pSrc);
}


#endif //RECOGNITION_OPENCV_UTILS_H
