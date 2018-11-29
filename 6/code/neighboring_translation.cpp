//
// Created by Miguel Chan on 2018/11/28.
//

#include "neighboring_translation.h"
#include <algorithm>
#include <exception>

using namespace std;

neighboring_translation::neighboring_translation(const vector<vector<int>> &adjacents): adjacents(adjacents) {}

//Assume that a middle element in the adjacents list should have two adjacent neighbor.
std::vector<int> neighboring_translation::write_out() {
    vector<int> result;
    //Find a one sided element to use as the start of the neighboring list.
    int currentIndex = 0;
    for (int i = 0; i < adjacents.size(); i++) {
        if (adjacents[i].size() == 1) {
            currentIndex = i;
            break;
        }
    }
    result.push_back(currentIndex);

    for (int i = 1; i < adjacents.size(); i++) {
        auto & nextIndexList = adjacents[currentIndex];
        for (auto index : nextIndexList) {
            //Add the new image index
            if (find(begin(result), end(result), index) == end(result)) {
                result.push_back(index);
                currentIndex = index;
                break;
            }
        }
    }
    return result;
}


