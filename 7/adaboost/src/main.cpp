#include <iostream>
#include "mnistData.h"
#include "adaboost.h"
#include <cstring>
#include <ctime>
#include <array>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

const char *boostPath[10] = {
        "./../mnist/boost0.xml",
        "./../mnist/boost1.xml",
        "./../mnist/boost2.xml",
        "./../mnist/boost3.xml",
        "./../mnist/boost4.xml",
        "./../mnist/boost5.xml",
        "./../mnist/boost6.xml",
        "./../mnist/boost7.xml",
        "./../mnist/boost8.xml",
        "./../mnist/boost9.xml",
};

double testBoost(array<adaboost, 10> &boosts) {
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

Mat getTestImage(const char* filename) {
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

int main(int argc, char **argv) {
    bool test = false;
    string filename;
    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        test = true;
        if (argc >= 3) {
            filename = argv[2];
        }
    }

    auto data = mnistData::getInstance();
    auto start = clock();

    if (!test) {
        array<adaboost, 10> boosts = {
                adaboost(), adaboost(),
                adaboost(), adaboost(),
                adaboost(), adaboost(),
                adaboost(), adaboost(),
                adaboost(), adaboost()
        };
        cout << "Training started... " << endl;
        auto sample = data->getTrainSet();
        for (int i = 0; i < 10; i++) {
            auto labels = data->getTrainBinaryLabels(i);
            boosts[i].train(sample, labels);
            cout << "Trained " << i + 1 << " Boosts. " << 10 - i - 1 << " to go." << endl;
            boosts[i].save(boostPath[i]);
        }
        cout << "Training complete. Correct rate: " << testBoost(boosts) << endl;
    } else {
        int total = data->getTestSize();
        int corrects = 0;
        adaboost boosts[10];
        for (int k = 0; k < 10; k++) {
            boosts[k].load(boostPath[k]);
        }
        if (filename.empty()) {
            for (int i = 0; i < total; i++) {
                int result = -1;
                auto testImg = data->getTestImageData(i);
                for (int k = 0; k < 10; k++) {
                    float predict = boosts[k].test(testImg);
                    if (predict != 0) {
                        result = k;
                    }
                }
                cout << "Predict: " << result << " Label: " << data->getTestLabel(i) << endl;
                if (result == 7 || result == 8 || result == 4 || result == 9) {
                    imshow("", data->getTestImage(i));
                    waitKey(0);
                }
                if (result == data->getTestLabel(i)) ++corrects;
            }
            cout << "Correct: " << (double) corrects / total * 100 << "%" << endl;
        } else {
            Mat test = getTestImage(filename.c_str());
            int result = -1;
            for (int k = 0; k < 10; k++) {
                float predict = boosts[k].test(test);
                if (predict != 0) {
                    result = k;
                }
            }
            cout << "Predict: " << result << endl;
            waitKey(0);
        }
    }
    cout << "Time: " << double(clock() - start) / CLOCKS_PER_SEC << "s." << endl;

    return 0;
}