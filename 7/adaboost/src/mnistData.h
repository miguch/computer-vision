//
// Created by Miguel Chan on 2018/12/16.
//

#ifndef NUMBER_DETECTION_MNISTDATA_H
#define NUMBER_DETECTION_MNISTDATA_H

#include <opencv2/opencv.hpp>
#include <memory>
#include <vector>

class mnistData {
private:
    static std::shared_ptr<mnistData> singleton;
    mnistData();
    std::vector<cv::Mat> trainImg;
    std::vector<cv::Mat> testImg;
    std::vector<unsigned char> trainLabels;
    std::vector<unsigned char> testLabels;
    cv::Mat trainSet;
    cv::Mat testSet;
    cv::Mat trainLabelMat;
    cv::Mat testLabelMat;
    std::vector<cv::Mat> trainBinaryLabels;
    std::vector<cv::Mat> testBinaryLabels;

public:
    cv::Mat getTestImage(int index);
    cv::Mat getTrainImage(int index);
    cv::Mat getTestImageData(int index);
    cv::Mat getTrainImageData(int index);
    int getTestLabel(int index);
    int getTrainLabel(int index);
    cv::Mat getTrainSet();
    cv::Mat getTestSet();
    cv::Mat getTrainLabelMat();
    cv::Mat getTestLabelMat();
    cv::Mat getTrainBinaryLabels(int index);
    cv::Mat getTestBinaryLabels(int index);
    cv::Mat getTrainPart(int startIndex, int size);
    cv::Mat getTrainLabelPart(int index, int startIndex, int size);
    int getTestSize();
    static std::shared_ptr<mnistData> getInstance();
    int getTrainSize();

};


#endif //NUMBER_DETECTION_MNISTDATA_H
