//
// Created by Miguel Chan on 2018/12/17.
//

#ifndef NUMBER_DETECTION_TESTUTILS_H
#define NUMBER_DETECTION_TESTUTILS_H
#include <opencv2/opencv.hpp>
#include <array>
#include "adaboost.h"

double testBoost(std::array<adaboost, 10> &boosts);

cv::Mat getTestImage(const char* filename);

void testA4(const char * filename, std::array<adaboost, 10> &boosts);

#endif //NUMBER_DETECTION_TESTUTILS_H
