//
// Created by Miguel Chan on 2018/12/16.
//

#include "adaboost.h"

#include <opencv2/opencv.hpp>
#include <iostream>


using namespace cv;


adaboost::adaboost(const cv::Mat & training, const cv::Mat & response): training(training),
                                                                        response(response) {
    initBoost();
}


void adaboost::train() {
    boost->train(training, ml::ROW_SAMPLE, response);
}

float adaboost::test(cv::Mat testCase) {
    return boost->predict(testCase);
}

adaboost::adaboost() {
    initBoost();
}

void adaboost::load(const char *path) {
    boost = boost->load(path);
}

void adaboost::save(const char *path) {
    boost->save(path);
}

void adaboost::initBoost() {
    boost = ml::Boost::create();
    boost->setBoostType(ml::Boost::LOGIT);
    boost->setWeakCount(100);
    boost->setWeightTrimRate(0.95);
    boost->setMaxDepth(2);
    boost->setUseSurrogates(false);
    boost->setPriors(Mat());
}

void adaboost::train(const cv::Mat &train, const cv::Mat &response) {
    boost->train(train, ml::ROW_SAMPLE, response);
}
