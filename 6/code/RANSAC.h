//
// Created by Miguel Chan on 2018/11/28.
//

#ifndef PANORAMA_RANSAC_H
#define PANORAMA_RANSAC_H


#include "feature_matching.h"
#include <vector>
#include <unordered_set>
#include <array>

class RANSAC {
public:
    typedef feature_matching::kPtPair kPtPair;
private:
    const std::vector<kPtPair> & kpPairs;

public:
    explicit RANSAC(const std::vector<feature_matching::kPtPair> &kpPairs);

    std::array<double, 8> run();

    typedef std::array<double, 8> homography_mat;
    static std::array<double, 8> getHomography(const std::vector<feature_matching::kPtPair> &pairs);

    static float getHomographyX(const homography_mat &mat, float x, float y);
    static float getHomographyY(const homography_mat &mat, float x, float y);

    std::vector<kPtPair> getInliners(const homography_mat &mat, std::unordered_set<int> indexes);

};


#endif //PANORAMA_RANSAC_H
