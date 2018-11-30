//
// Created by Miguel Chan on 2018/11/27.
//

#include <vector>
#include <iostream>
#include "feature_matching.h"

using namespace std;
extern "C" {
#include <vl/kdtree.h>
}

feature_matching::feature_matching(const feature_matching::featSet &a, const feature_matching::featSet &b): aFeat(a), bFeat(b) {}

const double thresh = 0.50;

std::vector<feature_matching::kPtPair> feature_matching::run_matching() {
    auto forest = vl_kdforest_new(VL_TYPE_FLOAT, 128, 1, VlDistanceL1);
    vector<kPtPair> result;

    vector<float> aFeatData;
    for (const auto & feat : aFeat) {
        const auto & descriptor = feat.first;

        copy(begin(descriptor), end(descriptor), back_inserter(aFeatData));

    }

    vl_kdforest_build(forest, aFeat.size(), aFeatData.data());

    auto searcher = vl_kdforest_new_searcher(forest);

    VlKDForestNeighbor neighbors[2];

    for (const auto & feat : bFeat) {

        const auto & featData = feat.first;

        vl_kdforestsearcher_query(searcher, neighbors, 2, featData.data());

        double ratio = neighbors[0].distance / neighbors[1].distance;
        if (ratio < thresh) {
            array<float, 128> target{};

            copy(begin(aFeatData) + neighbors[0].index * 128,
                 begin(aFeatData) + neighbors[0].index * 128 + 128,
                 begin(target));

            VlSiftKeypoint left = aFeat.at(target);
            VlSiftKeypoint right = feat.second;
            result.emplace_back(left, right);
        }
    }
    vl_kdforestsearcher_delete(searcher);
    vl_kdforest_delete(forest);

    return result;
}


