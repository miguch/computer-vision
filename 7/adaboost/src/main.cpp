#include <iostream>
#include "mnistData.h"
#include "adaboost.h"
#include "testUtils.h"
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

int main(int argc, char **argv) {
    bool test = false, a4 = false;
    string filename;
    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        test = true;
        if (argc >= 3) {
            filename = argv[2];
            if (argc >= 4 && strcmp(argv[3], "a4") == 0) {
                a4 = true;
            }
        }
    }

    auto data = mnistData::getInstance();
    auto start = clock();

    array<adaboost, 10> boosts = {
            adaboost(), adaboost(),
            adaboost(), adaboost(),
            adaboost(), adaboost(),
            adaboost(), adaboost(),
            adaboost(), adaboost()
    };

    if (!test) {
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
                if (result == data->getTestLabel(i)) ++corrects;
            }
            cout << "Correct: " << (double) corrects / total * 100 << "%" << endl;
        } else if (!a4) {
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
        } else {
            //A4 photo
            testA4(filename.c_str(), boosts);
        }
    }
    cout << "Time: " << double(clock() - start) / CLOCKS_PER_SEC << "s." << endl;

    return 0;
}