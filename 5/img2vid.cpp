//
// Created by Miguel Chan on 2018/11/8.
//

#include <iostream>
#include <functional>
#include <unordered_map>
#include <string>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

int main(int argc, char** argv) {
    if (argc == 1) {
        cerr << "Usage: " << argv[0] << " -i imgFile -o outputFile -f intermediateFrames -fps fps" << endl;
        exit(1);
    }
    int i, fps, frames;
    string imgFile, outputFile;
    bool fpsSet = false, framesSet = false, inputSet = false, outputSet = false;
    unordered_map<string, function<void()>> processor;
    auto stringFlagVar = [&](string & var, bool & set) -> function<void()> {
        return [&]() {
            if (i == argc - 1) {
                cerr << "Unexpected end of arguments." << endl;
                exit(1);
            }
            var = argv[++i];
            set = true;
        };
    };
    auto intFlagVar = [&](int & var, bool & set) -> function<void()> {
        return [&]() {
            if (i == argc - 1) {
                cerr << "Unexpected end of arguments." << endl;
                exit(1);
            }
            var = atoi(argv[++i]);
            set = true;
        };
    };
    processor["-i"] = stringFlagVar(imgFile, inputSet);
    processor["-o"] = stringFlagVar(outputFile, outputSet);
    processor["-f"] = intFlagVar(frames, framesSet);
    processor["-fps"] = intFlagVar(fps, fpsSet);
    for (i = 1; i < argc; i++) {
        if (processor.count(argv[i])) {
            processor[argv[i]]();
        } else {
            cerr << "Unknown argument: " << argv[i] << endl;
            exit(1);
        }
    }
    if (!(fpsSet && framesSet && inputSet && outputSet)) {
        cerr << "Not all required flags are set!" << endl;
        cerr << "Usage: " << argv[0] << " -i imgFile -o outputFile -f frames -fps fps" << endl;
        exit(1);
    }

    int totalFrames = frames + 2;
    CImg<unsigned char> img(imgFile.c_str());
    CImg<unsigned char> vid(img.width() / totalFrames, img.height(), totalFrames, 3, 0);
    cimg_forXYZ(vid, x, y, z) {
        for (int i = 0; i < 3; i++)
            vid(x, y, z, i) = img(x + z * vid.width(), y, i);
    }
    vid.save_ffmpeg_external(outputFile.c_str(), fps);
    cout << "Done." << endl;
}