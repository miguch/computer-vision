//
// Created by Miguel Chan on 2018/11/20.
//
#include <iostream>
#include "spherical_warp.h"
#include "feature_extract.h"
#include "utils.h"

using namespace std;

int main(int argc, char** argv) {
    CImg<unsigned char> a(argv[1]);

    a = utils::toGreyScale(a);
    feature_extract fe(a);
    auto re = fe.getOctaves();
    auto dog = fe.getDOGs();
    auto extremas = fe.getExtremas();
    auto ori = fe.getOrientations();
    auto mag = fe.getMagnitudes();

    for (int i  = 0; i < re.size(); i++) {
        re[i].save((to_string(i) + ".jpg").c_str());
    }

    for (int i = 0; i < dog.size(); i++) {
        dog[i].save((to_string(i) + "dog.jpg").c_str());
    }

    for (int i = 0; i < extremas.size(); i++) {
        extremas[i].save((to_string(i) + "ext.jpg").c_str());
    }

    for (int i = 0; i < ori.size(); i++) {
        ori[i].save((to_string(i) + "ori.jpg").c_str());
        mag[i].save((to_string(i) + "mag.jpg").c_str());
    }

}
