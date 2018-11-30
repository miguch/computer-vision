//
// Created by Miguel Chan on 2018/11/20.
//
#include <iostream>
#include <string>
#include <vector>
#include "spherical_warp.h"
#include "utils.h"
#include "feature_extraction.h"
#include "image_stitching.h"

using namespace std;

string pathJoin(const string& a, const string& b) {
#ifdef _WIN32
    return a + '\\' + b;
#else
    return a + '/' + b;
#endif
}



int main(int argc, char** argv) {
    int images;
    string baseDir = ".";
    if (argc == 1) {
        cerr << "\nUsage: " << argv[0] << " imageNumber [imageDirectory]" << endl;
        return 1;
    }
    if (argc > 1) {
        images = atoi(argv[1]);
    }
    if (argc > 2) {
        baseDir = argv[2];
    }

    vector<CImg<unsigned char>> sources;
    sources.reserve(images);

    for (int i = 0; i < images; i++) {
        string fullPath = pathJoin(baseDir, to_string(i) + ".jpg");
        auto img = CImg<unsigned char>(fullPath.c_str());
        //Make the image smaller to save some time
        if (img.height() > 1024)
            img.resize(1024 * img.width() / img.height(), 1024);
        sources.push_back(img);
    }

    auto pano = stitching::run_stitching(sources);

    pano.display();
    pano.save(pathJoin(baseDir.c_str(), "panorama.jpg").c_str());

}
