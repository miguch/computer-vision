#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>
#include <functional>
#include "HistoEqual.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Usage: " << argv[0] << " -i filename [{-l|--level} histogram level(default 256)] [-rgb(default grey scale)]" << endl;
        exit(1);
    }
    int i, histoLevel = 256;
    string filename;
    bool rgb = false;

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
    if (!rgb) {
        he.getGreyHistogram().display_graph("Grey scale histogram before processing", 3);
        auto greyEqualized = he.runWithGreyScale(histoLevel);
        greyEqualized.save("greyEqualized.bmp");
        greyEqualized.display("greyEqualized");
        greyEqualized.histogram(256).display_graph("Grey histogram after processing", 3);
    } else {
        he.getRGBHistogram().display_graph("RGB histogram before processing", 3);
        auto RGBEqualized = he.runWithRGB(histoLevel);
        RGBEqualized.save("RGBEqualized.bmp");
        RGBEqualized.display("RGBEqualized");
        RGBEqualized.histogram(256).display_graph("RGB histogram after processing", 3);
    }
    return 0;
}