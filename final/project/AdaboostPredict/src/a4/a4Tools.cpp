//
// Created by Miguel Chan on 2018/12/25.
//

#include "a4Tools.h"
#include "kmeans.h"
#include "regionGrow.h"
#include <iostream>
#include "src/utils.h"
#include "houghTransform.h"
#include "a4Warping.h"

using namespace std;

CImg<unsigned char>
a4Tools::adjustA4Image(const CImg<unsigned char> &img, const char* debugMessage, bool saveKmeans, bool saveRegionGrow, bool saveOutline,
              bool saveIntersects, bool saveResult) {
    const int k = 2;

    auto blured = img.get_blur(16);

    kmeans km(blured);

    cout << "Performing kmeans " << debugMessage << endl;

    auto k_mean = km.runKmeans(k, true);
    if (saveKmeans) {
        k_mean.save("kmeans.jpg");
    }

    cout << " Performing Region Grow " << debugMessage << endl;

    //Eliminate papers not at the center of the image
    //threshold is one because k_mean is already a image with only 255 and 0
    auto grow = runRegionGrow(k_mean, 1);
    if (saveRegionGrow) {
        grow.save("region-grow.jpg");
    }

    auto outline = utils::getOutline(grow);
    if (saveOutline) {
        outline.save("lines.jpg");
    }

    cout << "Performing Hough Transform " << debugMessage << endl;
    auto lines = houghTransform::line_transform(outline, 0.25);

    auto kblines = houghTransform::polarToCartesian(lines);
//    for (auto p : kblines) {
//        cout << p.first << " " << p.second << endl;
//    }

    auto intersects = houghTransform::findIntersect(outline, kblines);

    CImg<unsigned char> canvas = img;
    houghTransform::markEdges(kblines, intersects, canvas);
    houghTransform::drawCornorPoints(intersects, canvas);

    if (saveIntersects) {
        canvas.save("intersects.jpg");
    }

    cout << "Warping... " << debugMessage << endl;

    a4Warping warper(img);

    auto a4 = warper.runWarping(intersects);
    if (saveResult) {
        a4.save("a4.jpg");
    }

    return a4;
}
