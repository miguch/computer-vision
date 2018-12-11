#include <iostream>
#include <unordered_map>
#include <functional>
#include "CImg.h"
#include "kmeans.h"
#include "regionGrow.h"
#include "utils.h"
#include "houghTransform.h"
#include "a4Warping.h"
#include <string>

using namespace cimg_library;
using namespace std;

int main(int argc, char** argv) {

    int i = 1, k = 2;
    unordered_map<string, function<void()>> arg_processor;
    auto intVar = [&](string arg, int& target) {
        return [&]() {
            ++i;
            if (argc - 1 == i) {
                cerr << "Argument " << arg << " value not specified." << endl;
                exit(1);
            }
            target = atoi(argv[i]);
        };
    };
    arg_processor["-k"] = intVar("-k", k);

    for (i = 1; i < argc - 1; i++) {
        if (arg_processor.count(argv[i])) {
            arg_processor[argv[i]]();
        } else {
            cerr << "Unknown argument " << argv[i] << endl;
            return 1;
        }
    }

    if (argc == 1) {
        cerr << "No file specified!" << endl;
        return 1;
    }

    const char* filename = argv[argc - 1];

    cout << "k = " <<  k << endl;

    CImg<unsigned char> src(filename);
    auto blured = src.get_blur(16);

    kmeans km(blured);

    cout << "Performing kmeans" << endl;

    auto k_mean = km.runKmeans(k, true);
    k_mean.save("kmeans.jpg");

    cout << "Performing Region Grow" << endl;

    //Eliminate papers not at the center of the image
    //threshold is one because k_mean is already a image with only 255 and 0
    auto grow = runRegionGrow(k_mean, 1);
    grow.save("region-grow.jpg");

    auto outline = utils::getOutline(grow);
    
    cout << "Performing Hough Transform" << endl;
    auto lines = houghTransform::line_transform(outline, 0.4);

    auto kblines = houghTransform::polarToCartesian(lines);

    auto intersects = houghTransform::findIntersect(outline, kblines);

    CImg<unsigned char> canvas = src;
    houghTransform::markEdges(kblines, intersects, canvas);
    houghTransform::drawCornorPoints(intersects, canvas);

    canvas.save("outlined.jpg");

    cout << "Warping..." << endl;

    a4Warping warper(src);

    auto a4 = warper.runWarping(intersects);
    a4.save("a4.jpg");

    a4.display();


    return 0;
}