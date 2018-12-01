//
// Created by Miguel Chan on 2018/11/28.
//

#include "RANSAC.h"
#include "CImg.h"
#include <unordered_set>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace cimg_library;

RANSAC::RANSAC(const std::vector<RANSAC::kPtPair> &kpPairs): kpPairs(kpPairs) {
    srand(static_cast<unsigned int>(time(nullptr)));
}


const double expectedProbability = 0.99, outlierRatio = 0.3;
const int subsetSize = 6;
const int iterations = log(1 - expectedProbability) / log(1 - pow(outlierRatio, subsetSize));
std::array<double, 8> RANSAC::run() {
    vector<kPtPair> maxInliners;

    for (int i = 0; i < iterations; i++) {
        vector<kPtPair> pairs;
        unordered_set<int> indexes;

        for (int k = 0; k < subsetSize; k++) {
            int randomIndex;
            do {
                randomIndex = rand() % kpPairs.size();
            } while (indexes.count(randomIndex));

            indexes.insert(randomIndex);
            pairs.push_back(kpPairs[randomIndex]);
        }

        homography_mat mat = getHomography(pairs);

        auto inliners = getInliners(mat, indexes);

        if (inliners.size() > maxInliners.size()) {
            maxInliners = inliners;
        }

    }

    //Use the inliners to re-calculate solution
    return getHomography(maxInliners);

}

//Use CImg to calculate homography matrix
std::array<double, 8> RANSAC::getHomography(const std::vector<feature_matching::kPtPair> &pairSet) {

    int size = pairSet.size();
    //a homography matrix can be computed with 4 pixels
    CImg<double> A(4, size, 1, 1, 0), bx(1, size, 1, 1, 0), by(1, size, 1, 1, 0);

    for (int i = 0; i < size; i++) {
        A(0, i) = pairSet[i].first.x;
        A(1, i) = pairSet[i].first.y;
        A(2, i) = pairSet[i].first.x * pairSet[i].first.y;
        A(3, i) = 1;
        bx(0, i) = pairSet[i].second.x;
        by(0, i) = pairSet[i].second.y;
    }

    CImg<double> x = bx.get_solve(A);
    CImg<double> y = by.get_solve(A);

    return {x(0, 0), x(0, 1), x(0, 2), x(0, 3), y(0, 0), y(0, 1), y(0, 2), y(0, 3)};
}

double RANSAC::getHomographyX(const RANSAC::homography_mat &mat, double x, double y) {
    return mat[0] * x + mat[1] * y + mat[2] * x * y + mat[3];
}

double RANSAC::getHomographyY(const RANSAC::homography_mat &mat, double x, double y) {
    return mat[4] * x + mat[5] * y + mat[6] * x * y + mat[7];
}


double ransacThresh = 4.0;
vector<RANSAC::kPtPair> RANSAC::getInliners(const RANSAC::homography_mat &mat, std::unordered_set<int> indexes) {
    vector<kPtPair> result;
    for (int ind = 0; ind < kpPairs.size(); ind++) {
        if (indexes.count(ind)) {
            continue;
        }

        double x = kpPairs[ind].second.x;
        double y = kpPairs[ind].second.y;

        double ax = getHomographyX(mat, kpPairs[ind].first.x, kpPairs[ind].first.y);
        double ay = getHomographyY(mat, kpPairs[ind].first.x, kpPairs[ind].first.y);

        double dist = sqrt(pow(ax - x, 2) + pow(ay - y, 2));

        if (dist < ransacThresh) {
            result.push_back(kpPairs[ind]);
        }
    }
    return result;

}


