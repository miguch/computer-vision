#include <iostream>
#include "mnistData.h"
#include "adaboost.h"
#include <cstring>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

const char* boostPath[4] = {
        "./../mnist/boost1.xml",
        "./../mnist/boost2.xml",
        "./../mnist/boost3.xml",
        "./../mnist/boost4.xml",
};

int main(int argc, char** argv) {
    bool test = false;
    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        test = true;
    }

    auto data = mnistData::getInstance();

    if (!test) {
        cout << "Training started... " << endl;
        for (int i = 0; i < 4; i++) {
            adaboost boost(data->getTrainSet(), data->getTrainBinaryLabels(i));
            boost.train();
            boost.save(boostPath[i]);
            cout << "Trained " << i + 1 << " Boost, " << 4 - i - 1 << " to go." << endl;
        }
    } else {
        int total = data->getTestSize();
        int corrects = 0;
        adaboost boosts[4];
        for (int k = 0; k < 4; k++) {
            boosts[k].load(boostPath[k]);
        }
        for (int i = 0; i < total; i++) {
            int result = 0;
            auto testImg = data->getTestImageData(i);
            for (int k = 0; k < 4; k++) {
                float predict = boosts[k].test(testImg);
                result += pow(2, k) * predict;
            }
            cout << "Predict: " << result << " Label: " << data->getTestLabel(i) << endl;
            if (result == data->getTestLabel(i)) ++corrects;
        }
        cout << "Correct: " << (double)corrects / total * 100 << "%" << endl;
    }

    return 0;
}