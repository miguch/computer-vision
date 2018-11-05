#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>
#include <functional>
#include "ColorTransfer.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Usage: " << argv[0] << " -s filename -t filename" << endl;
        exit(1);
    }
    int i;
    string source, target;

    //Process args
    unordered_map<string, std::function<void()>> processor;
    processor["-s"] = [&]() {
        if (i == argc - 1) {
            cerr << "No input file specified!" << endl;
            exit(1);
        } else {
            source = argv[++i];
        }
    };
    processor["-t"] = [&]() {
        if (i == argc - 1) {
            cerr << "No input file specified!" << endl;
            exit(1);
        } else {
            target = argv[++i];
        }
    };
    for (i = 1; i < argc; i++) {
        if (processor.count(argv[i]) == 0) {
            cerr << "Unknown argument: " << argv[i] << endl;
            exit(1);
        }
        processor[argv[i]]();
    }
    if (source == "" || target == "") {
        cerr << "You must specify a source and a target file!" << endl;
        exit(1);
    }

    ColorTransfer ct(source, target);
    auto result = ct.run();
    result.save("transfer.bmp");
    ct.getSource().append(ct.getTarget()).append(result).display("Image after transfer");

    return 0;
}