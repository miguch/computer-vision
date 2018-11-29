//
// Created by Miguel Chan on 2018/11/27.
//

#ifndef PANORAMA_FEATURE_MATCHING_H
#define PANORAMA_FEATURE_MATCHING_H


#include <cstdint>
#include <map>
#include <array>
#include <vector>

extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
}

class feature_matching {
private:
    typedef std::map<std::array<float, 128>, VlSiftKeypoint> featSet;
    const featSet &aFeat, &bFeat;
public:
    feature_matching(const featSet& a, const featSet& b);

    typedef std::pair<VlSiftKeypoint, VlSiftKeypoint> kPtPair;

    std::vector<kPtPair> run_matching();
};


#endif //PANORAMA_FEATURE_MATCHING_H
