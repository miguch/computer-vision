//
// Created by Miguel Chan on 2018/12/26.
//

#ifndef RECOGNITION_ADABOOSTTOOLS_H
#define RECOGNITION_ADABOOSTTOOLS_H

#include "adaboost.h"
#include <array>

class adaboostTools {
private:
    std::array<adaboost, 10> boosts;

public:
    adaboostTools();

    int predictImg(cv::Mat testCase);

};


#endif //RECOGNITION_ADABOOSTTOOLS_H
