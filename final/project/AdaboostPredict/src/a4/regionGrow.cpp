//
// Created by Miguel Chan on 2018/12/11.
//

#include "regionGrow.h"
#include <stack>
#include <iostream>

using namespace std;

typedef pair<int, int> point;

//Performs region grow on a image with threshold
//Used to eliminate all other a4 papers on the image in this project.
CImg<unsigned char> runRegionGrow(const CImg<unsigned char> &src, unsigned char threshold) {
    CImg<unsigned char> result(src.width(), src.height(), 1, 1, 0);
    CImg<bool> used(src.width(), src.height(), 1, 1, false);

    stack<point> seeds;
    seeds.push({src.width() / 2, src.height() / 2});
    //The middle points of the image is the first seed.

    while (!seeds.empty()) {
        point seed = seeds.top();
        seeds.pop();
        result(seed.first, seed.second) = 255;
        //Check eight neighbors, add all neighbors that has difference lower than threshold
        for (int i = -1; i <= 1; i++) {
            for (int k = -1; k <= 1; k++) {
                if (i == 0 && k == 0)
                    continue;
                int newX = seed.first + i;
                int newY = seed.second + k;
                if (newX < 0 || newX >= src.width() || newY < 0 || newY >= src.height())
                    continue;
                if(used(newX, newY))
                    continue;
                if (abs(src(seed.first, seed.second) - src(newX, newY)) <= threshold) {
                    seeds.push({newX, newY});
                }
                used(newX, newY) = true;
            }
        }
    }
    return result;

}
