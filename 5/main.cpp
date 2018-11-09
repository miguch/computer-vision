#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include "ImageMorphing.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc == 1) {
        cerr << "Usage: " << argv[0] << " -s filename -d filename [{--savepoints|--loadpoints} filename] [--edit] [-f frames] [-fps fps]" << endl;
        exit(1);
    }
    string sourceFile, targetFile, loadPointFile, savePointFile;
    bool edit = false;
    int i;
    int frames = 11, fps = 25;

    //Process args
    unordered_map<string, function<void()>> processor;
    auto stringFlagVar = [&](string & var) -> function<void()> {
        return [&]() {
            if (i == argc - 1) {
                cerr << "Unexpected end of arguments." << endl;
                exit(1);
            }
            var = argv[++i];
        };
    };
    auto intFlagVar = [&](int & var) -> function<void()> {
        return [&]() {
            if (i == argc - 1) {
                cerr << "Unexpected end of arguments." << endl;
                exit(1);
            }
            var = atoi(argv[++i]);
        };
    };
    processor["-s"] = stringFlagVar(sourceFile);
    processor["-d"] = stringFlagVar(targetFile);
    processor["--savepoints"] = stringFlagVar(savePointFile);
    processor["--loadpoints"] = stringFlagVar(loadPointFile);
    processor["--edit"] = [&]() {
        edit = true;
    };
    processor["-f"] = intFlagVar(frames);
    processor["-fps"] = intFlagVar(fps);

    for (i = 1; i < argc; i++) {
        if (processor.count(argv[i]) == 0) {
            cerr << "Undefined argument: " << argv[i] << endl;
            exit(1);
        }
        processor[argv[i]]();
    }
    if (sourceFile == "") {
        cerr << "Source image is not set!" << endl;
        exit(1);
    }
    if (targetFile == "") {
        cerr << "Target image is not set!" << endl;
        exit(1);
    }
    if (loadPointFile != "" && savePointFile != "") {
        cerr << "You can only choose whether to save or load points from file!" << endl;
        exit(1);
    }
    if (edit && loadPointFile == "") {
        cerr << "To edit, you must specify an input load file!" << endl;
        exit(1);
    }

    //Image morphing
    ImageMorphing im(sourceFile, targetFile);
    vector<pair<ImageMorphing::point, ImageMorphing::point>> points;
    if (loadPointFile != "") {
        points = im.readPointsFromFile(loadPointFile);
        if (edit) {
            points = im.selectPoints(points);
            im.savePointToFile(points,loadPointFile);
        }
    } else {
        points = im.selectPoints();
        if (savePointFile != "") {
            im.savePointToFile(points,savePointFile);
        }
    }

    auto res = im.morph(points, frames);
    //add reverse ordered frames
    for (int i = res.second.size() - 1; i >= 0; i--) {
        res.second.push_back(res.second(i));
    }
    res.first.save("result.bmp");
    res.second.save_ffmpeg_external("result.mp4", fps);
    cout << "Done, result written to result.mp4" << endl;
    return 1;
}