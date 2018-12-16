//
// Created by Miguel Chan on 2018/12/16.
//

#ifndef NUMBER_DETECTION_ADABOOST_H
#define NUMBER_DETECTION_ADABOOST_H

#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/core/core.hpp>

class adaboost {
private:
    const cv::Mat training, response;
    cv::Ptr<cv::ml::Boost> boost;
    cv::Ptr<cv::ml::SVM> svm;
    void initBoost();

public:
    adaboost(const cv::Mat&, const cv::Mat&);
    adaboost();
    void train();
    void train(const cv::Mat& train, const cv::Mat& response);
    float test(cv::Mat testCase);
    void save(const char* path);
    void load(const char* path);
};


#endif //NUMBER_DETECTION_ADABOOST_H
