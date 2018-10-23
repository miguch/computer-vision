//
// Created by Miguel Chan on 2018/10/18.
//

#include <iostream>
#include <string>
#include "A4Detect.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc > 5) {
        cerr << "Error: extra program arguments." << endl << endl;
        cerr << "Usage: " << argv[0] << " [-t thresh(default 0.5) cannyThresh(default 50)] [filename]" << endl;
        exit(1);
    }
    string file;
    float thresh = 0.5;
    int cannyThresh = 50;
    bool fileSet = false;
    if (argc >= 2) {
        if (strncmp(argv[1], "-t", 2) == 0) {
            char* threshStr = argv[2];
            thresh = atof(threshStr);
            cout << "Using " << thresh << " as threshold for hough transform." << endl;
            char* cannyThrStr = argv[3];
            cannyThresh = atoi(cannyThrStr);
            cout << "Using " << cannyThresh << " as threshold for Canny." << endl;
            if (argc == 5) {
                file = argv[4];
                fileSet = true;
            }
        } else {
            file = argv[1];
            fileSet = true;
        }
    }
    if (!fileSet){
        cout << "Enter input filename: ";
        cin >> file;
    }
    A4Detect a(file, thresh, cannyThresh, true);
    a.runHough().display();
}