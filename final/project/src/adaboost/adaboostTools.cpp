//
// Created by Miguel Chan on 2018/12/26.
//

#include "adaboostTools.h"


const char *boostPath[10] = {
        "mnist/boost0.xml",
        "mnist/boost1.xml",
        "mnist/boost2.xml",
        "mnist/boost3.xml",
        "mnist/boost4.xml",
        "mnist/boost5.xml",
        "mnist/boost6.xml",
        "mnist/boost7.xml",
        "mnist/boost8.xml",
        "mnist/boost9.xml",
};


adaboostTools::adaboostTools() {
    for (int i = 0; i < 10; i++) {
        boosts[i].load(boostPath[i]);
    }

}

int adaboostTools::predictImg(cv::Mat testCase) {
    int result = -1;
    for (int k = 0; k < 10; k++) {
        float predict = boosts[k].test(testCase);
        if (predict != 0) {
            result = k;
        }
    }
    return result;
}
