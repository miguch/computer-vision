#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>
#include <functional>
#include "HistoEqual.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Usage: " << argv[0] << " -i filename [-l|--level histogram level(default 256)] [-rgb] [-ycbcr]" << endl;
        exit(1);
    }
    int i, histoLevel = 256;
    string filename;
    bool rgb = false, ycbcr = false;

    //Process args
    unordered_map<string, std::function<void()>> processor;
    processor["-i"] = [&]() {
        if (i == argc - 1) {
            cerr << "No input file specified!" << endl;
            exit(1);
        } else {
            filename = argv[++i];
        }
    };
    processor["--level"] = processor["-l"] = [&]() {
        if (i == argc - 1) {
            cerr << "Unexpected end of arguments!" << endl;
            exit(1);
        } else {
            histoLevel = atoi(argv[++i]);
        }
    };
    processor["-rgb"] = [&]() { rgb = true; };
    processor["-ycbcr"] = [&]() { ycbcr = true; };
    for (i = 1; i < argc; i++) {
        if (processor.count(argv[i]) == 0) {
            cerr << "Unknown argument: " << argv[i] << endl;
            exit(1);
        }
        processor[argv[i]]();
    }
    if (filename == "") {
        cerr << "You must specifiy a input file!" << endl;
        exit(1);
    }

    HistoEqual he(filename);
    //Histogram Equalization
    if (rgb && ycbcr) {
        auto RGBEqualized = he.runWithRGB(histoLevel);
        RGBEqualized.save("RGBEqualized.bmp");
        auto YCbCrEqualized = he.runWithYCbCr(histoLevel);
        YCbCrEqualized.save("YCbCrEqualized.bmp");
        he.getOriginImage().append(RGBEqualized).append(YCbCrEqualized).display("Equalized");
    } else if (rgb) {
        auto RGBEqualized = he.runWithRGB(histoLevel);
        RGBEqualized.save("RGBEqualized.bmp");
        he.getOriginImage().append(RGBEqualized).display("RGBEqualized");
    } else if (ycbcr) {
        auto Equalized = he.runWithYCbCr(histoLevel);
        Equalized.save("YCbCrEqualized.bmp");
        he.getOriginImage().append(Equalized).display("YCbCrEqualized");
    } else {
        he.getGreyHistogram().display_graph("Grey scale histogram before processing", 3);
        auto greyEqualized = he.runWithGreyScale(histoLevel);
        greyEqualized.save("greyEqualized.bmp");
        he.getGreyScale().append(greyEqualized).display("greyEqualized");
        greyEqualized.histogram(256).display_graph("Grey histogram after processing", 3);
    }
    return 0;
}