//
// Created by Miguel Chan on 2018/12/16.
//

#include "mnistData.h"
#include <cstdlib>
#include <array>
#include <fstream>

using namespace std;
using namespace cv;

shared_ptr<mnistData> mnistData::singleton = nullptr;

const char* trainImagePath = "./../mnist/train-images-idx3-ubyte";
const char* testImagePath = "./../mnist/t10k-images-idx3-ubyte";
const char* trainLabelPath = "./../mnist/train-labels-idx1-ubyte";
const char* testLabelPath = "./../mnist/t10k-labels-idx1-ubyte";

std::shared_ptr<mnistData> mnistData::getInstance() {
    if (singleton == nullptr) {
        singleton = shared_ptr<mnistData>(new mnistData);
    }
    return singleton;
}

void checkFileStatus(ifstream& file, const string &filename) {
    if (!file.is_open()) {
        cerr << "Cannot open file " << filename << endl;
        exit(1);
    }
}

int reverseInt(int i) {
    unsigned char c1, c2, c3, c4;
    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;
    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}


void readImgFile(ifstream& imgFile, vector<Mat>& imgList, Mat& imgSet) {
    int32_t magicNumber, imgSize, rows, cols;
    imgFile.read((char*)&magicNumber, 4);
    imgFile.read((char*)&imgSize, 4);
    imgFile.read((char*)&rows, 4);
    imgFile.read((char*)&cols, 4);
    magicNumber = reverseInt(magicNumber);
    imgSize = reverseInt(imgSize);
    rows = reverseInt(rows);
    cols = reverseInt(cols);
    int imgPixels = rows * cols;
    imgSet = Mat::zeros(imgSize, imgPixels, CV_32FC1);
    for (int i = 0; i < imgSize; i++) {
        Mat img = Mat::zeros(rows, cols, CV_8U);
        imgFile.read((char*)img.data, imgPixels);
        Mat temp;
        img.convertTo(temp, CV_32FC1);
        imgList.push_back(temp);
        memcpy(imgSet.data + i * imgPixels * sizeof(float), temp.data, imgPixels * sizeof(float));
    }
    imgSet = imgSet / 255;
}

array<unsigned char, 4> digitToBinary(unsigned char digit) {
    return {static_cast<unsigned char>(digit / 8 % 2), static_cast<unsigned char>(digit / 4 % 2),
            static_cast<unsigned char>(digit / 2 % 2), static_cast<unsigned char>(digit % 2)};
};

void readLabelFile(ifstream& labelFile, vector<unsigned char>& labelList, Mat& labelMat, std::vector<cv::Mat>& binaryLabels) {
    int32_t magicNumber, imgSize;
    labelFile.read((char*)&magicNumber, 4);
    labelFile.read((char*)&imgSize, 4);
    magicNumber = reverseInt(magicNumber);
    imgSize = reverseInt(imgSize);
    array<vector<unsigned char>, 10> labelSet;
    for (int i = 0; i < imgSize; i++) {
        unsigned char label;
        labelFile.read((char*)&label, 1);
        labelList.push_back(label);
        for (int k = 0; k < 10; k++) {
            if (k == label) {
                labelSet[k].push_back(1);
            } else {
                labelSet[k].push_back(0);
            }
        }
    }
    auto labels = Mat(imgSize, 1, CV_8UC1, labelList.data());
    labels.convertTo(labelMat, CV_32SC1);
    for (int i = 0; i < 10; i++) {
        auto bLabels = Mat(imgSize, 1, CV_8UC1, labelSet[i].data());
        Mat temp;
        bLabels.convertTo(temp, CV_32SC1);
        binaryLabels.push_back(temp);
    }
}


mnistData::mnistData() {
    ifstream trainImgFile(trainImagePath, ios::binary);
    ifstream testImgFile(testImagePath, ios::binary);
    ifstream trainLabelFile(trainLabelPath, ios::binary);
    ifstream testLabelFile(testLabelPath, ios::binary);

    readImgFile(trainImgFile, trainImg, trainSet);
    readImgFile(testImgFile, testImg, testSet);

    readLabelFile(trainLabelFile, trainLabels, trainLabelMat, trainBinaryLabels);
    readLabelFile(testLabelFile, testLabels, testLabelMat, testBinaryLabels);
}

cv::Mat mnistData::getTestImage(int index) {
    return testImg[index];
}

cv::Mat mnistData::getTrainImage(int index) {
    return trainImg[index];
}

int mnistData::getTestLabel(int index) {
    return testLabels[index];
}

int mnistData::getTrainLabel(int index) {
    return trainLabels[index];
}

cv::Mat mnistData::getTrainSet() {
    return trainSet;
}

cv::Mat mnistData::getTestSet() {
    return testSet;
}

cv::Mat mnistData::getTrainLabelMat() {
    return trainLabelMat;
}

cv::Mat mnistData::getTestLabelMat() {
    return testLabelMat;
}

cv::Mat mnistData::getTrainImageData(int index) {
    auto img = getTrainImage(index);
    int size = img.cols * img.rows;
    return Mat(1, size, CV_32FC1, img.data) / 255;
}

cv::Mat mnistData::getTestImageData(int index) {
    auto img = getTestImage(index);
    int size = img.cols * img.rows;
    return Mat(1, size, CV_32FC1, img.data) / 255;
}

cv::Mat mnistData::getTestBinaryLabels(int index) {
    return testBinaryLabels[index];
}

cv::Mat mnistData::getTrainBinaryLabels(int index) {
    return trainBinaryLabels[index];
}

int mnistData::getTestSize() {
    return static_cast<int>(testLabels.size());
}

cv::Mat mnistData::getTrainPart(int startIndex, int size) {
    Mat result(size, trainSet.cols, CV_32FC1);
    memcpy(result.data, trainSet.data + startIndex * trainSet.cols,
           size * trainSet.cols * sizeof(float));
    return result;
}

cv::Mat mnistData::getTrainLabelPart(int index, int startIndex, int size) {
    auto binary = trainBinaryLabels[index];
    Mat result(size, 1, CV_32SC1);
    memcpy(result.data, binary.data + startIndex,
           size * sizeof(int));
    return result;
}

int mnistData::getTrainSize() {
    return static_cast<int>(trainLabels.size());
}
