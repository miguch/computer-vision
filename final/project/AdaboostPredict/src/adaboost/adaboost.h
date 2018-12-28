//
// Created by Miguel Chan on 2018/12/26.
//

#ifndef RECOGNITION_ADABOOST_H
#define RECOGNITION_ADABOOST_H

#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>


class adaboost {
private:
    const cv::Mat training, response;
    cv::Ptr<cv::ml::Boost> boost;
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


#endif //RECOGNITION_ADABOOST_H
