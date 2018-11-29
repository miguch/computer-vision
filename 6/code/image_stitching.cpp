//
// Created by Miguel Chan on 2018/11/28.
//
#include <iostream>
#include <queue>
#include <list>
#include <unordered_set>
#include "image_stitching.h"
#include "utils.h"
#include "feature_extraction.h"
#include "feature_matching.h"
#include "spherical_warp.h"
#include "blending.h"
#include "neighboring_translation.h"
#include "RANSAC.h"

using namespace std;

namespace stitching {

    CImg<unsigned char> run_stitching(std::vector<CImg<unsigned char>> sources) {

        vector<map<array<float, 128>, VlSiftKeypoint>> features;
        std::vector<CImg<unsigned char>> greys;
        std::vector<CImg<unsigned char>> warped;
        greys.reserve(sources.size());
        warped.reserve(sources.size());

        for (int i = 0; i < sources.size(); i++) {
            cout << "\r Extracting feature of image " << i << flush;
            spherical_warp sw(sources[i]);
            warped.push_back(sw.run_warping());
            greys.push_back(utils::toGreyScale(warped[i]));

            feature_extraction fe(greys[i]);

            features.push_back(fe.run_extraction());
        }
        cout << endl << "Extraction completed" << endl;

        vector<vector<int>> adjacents(sources.size(), vector<int>());

        cout << endl << "Matching adjacent image..." << endl;

        for (int i = 0; i < sources.size(); i++) {
            for (int j = 0; j < sources.size(); j++) {
                if (i == j) {
                    continue;
                }
                feature_matching fm(features[i], features[j]);

                auto kPtPairs = fm.run_matching();

                if (kPtPairs.size() >= 15) {
                    cout << " Matched adjacent picture " << i << ", " << j << endl;
                    adjacents[i].push_back(j);
                }
            }
        }

        cout << endl << "Matching completed. Stitching..." << endl;
        neighboring_translation nt(adjacents);
        auto neighborList = nt.write_out();
        cout << "A possible Image list: ";
        for (auto i : neighborList) {
            cout << i << " ";
        }
        cout << endl;
        int middleIndex = neighborList[neighborList.size() / 2];
        queue<int> unvisited;
        unvisited.push(middleIndex);
        auto stitched = warped[middleIndex];
        int prevIndex = middleIndex;

        while (!unvisited.empty()) {
            int currentIndex = unvisited.front();
            unvisited.pop();
            vector<int>& adj = adjacents[currentIndex];

            //Stitch all that need to be stitched
            while (!adj.empty()) {
                int newIndex = adj.front();
                adj.erase(begin(adj));
                auto anoIter = find(begin(adjacents[newIndex]), end(adjacents[newIndex]), currentIndex);
                if (anoIter != end(adjacents[newIndex])) {
                    adjacents[newIndex].erase(anoIter);
                }
                unvisited.push(newIndex);
                feature_matching fm1(features[currentIndex], features[newIndex]);
                auto src2dst = fm1.run_matching();

                feature_matching fm2(features[newIndex], features[currentIndex]);
                auto dst2src = fm2.run_matching();

                if (src2dst.size() > dst2src.size()) {
                    dst2src.clear();
                    for (auto p : src2dst) {
                        dst2src.push_back(make_pair(p.second, p.first));
                    }
                } else {
                    src2dst.clear();
                    for (auto p : dst2src) {
                        src2dst.push_back(make_pair(p.second, p.first));
                    }
                }

                //Reverse
                RANSAC ranAB(dst2src), ranBA(src2dst);

                auto matAB = ranAB.run();
                auto matBA = ranBA.run();

                for (auto ele : matAB) {
                    cout << ele << " ";
                }
                cout << endl;

                blending bl(stitched, warped[newIndex], matAB, matBA);

                stitched = bl.run();

                bl.updateFeatureWithMat(features[newIndex]);
                bl.updateFeatureWithOffset(features[prevIndex]);

                prevIndex = newIndex;

                cout << " Stitched image " << currentIndex << " and " << newIndex << endl;
                //stitched.display();
            }



        }

        return stitched;

    }
}