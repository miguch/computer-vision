//
// Created by Miguel Chan on 2018/10/21.
//

#include <iostream>
#include "CoinDetect.h"

using namespace std;

int main(int argc, char **argv) {
    if (argc > 7) {
        cerr << "Error: extra program arguments." << endl << endl;
        cerr << "Usage: " << argv[0] << " [-t thresh(default 0.5) cannyThresh(default 50)] filename low_r high_r"
             << endl;
        exit(1);
    }
    if (argc != 4 && argc != 7) {
        cerr << "Error: program arguments not valid." << endl << endl;
        cerr << "Usage: " << argv[0] << " [-t thresh(default 0.5) cannyThresh(default 50)] filename low_r high_r"
             << endl;
        exit(1);
    }
    string file;
    float thresh = 0.5;
    int cannyThresh = 50;
    int low_r, high_r;
    if (strncmp(argv[1], "-t", 2) == 0) {
        char *threshStr = argv[2];
        thresh = atof(threshStr);
        cout << "Using " << thresh << " as threshold for hough transform." << endl;
        char *cannyThrStr = argv[3];
        cannyThresh = atoi(cannyThrStr);
        cout << "Using " << cannyThresh << " as threshold for Canny." << endl;
        file = argv[4];
        low_r = atoi(argv[5]);
        high_r = atoi(argv[6]);
    } else {
        file = argv[1];
        low_r = atoi(argv[5]);
        high_r = atoi(argv[6]);
    }
    CoinDetect cd(thresh, cannyThresh, file, low_r, high_r);
    auto result = cd.runHough();
    result.display();
    result.save("coins.bmp");
    return 0;
}
