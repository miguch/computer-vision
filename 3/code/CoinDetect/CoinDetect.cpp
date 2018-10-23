//
// Created by Miguel Chan on 2018/10/21.
//

#include "CoinDetect.h"
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif


//Used in order to make std::array<int, 3> a key for hash table(unordered_map)
struct CircleHasher {
    std::size_t operator()(const std::array<int, 3>& a) const{
        std::size_t h = 0;

        for (auto e : a) {
            h ^= std::hash<int>{}(e)  + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};


CoinDetect::CoinDetect(double houghThresh, int cannyThresh,
                       string filename, int lowR, int highR): cannyThresh(cannyThresh),
                                         houghThreshPercent(houghThresh),
                                         origin(filename.c_str()),
                                         _filename(filename),
                                         low_r(lowR),
                                         high_r(highR){}

const int CoinDetect::houghSpaceSlice = 360;
const double CoinDetect::interval = 2 * M_PI / CoinDetect::houghSpaceSlice;


/**Run hough transform to detect circles
 * @return the original image with coins marked.
 */
CImg<unsigned char> CoinDetect::runHough() {
    cout << "Processing image: " << _filename << endl;
    cout << "Width: " << origin.width() << endl;
    cout << "Height: "  << origin.height() << endl;
    Canny c(_filename, false, cannyThresh, cannyThresh, 5, 4);
    auto edges = c.runCanny();
    edges.save("cannyEdges.bmp");
    auto circles = getHoughCircles(edges);
    for (auto & c : circles) {
        for (auto e : c) {
            cout << e << " ";
        }
        cout << endl;
    }
    cout << "Total coins: " << circles.size() << endl;
    drawCircle(circles, origin);
    return origin;
}

/***************Returns all detected circles as vector of (x, y, r)**********/
vector<CoinDetect::circle> CoinDetect::getHoughCircles(const CImg<unsigned char>& edges) {

    int max_x = edges.width(), max_y = edges.height();
    double maxLength = sqrt(pow(max_y, 2) + pow(max_x, 2));
    //Set the lower and higher bounds of r
    int total_r = high_r - low_r;

    cout << "Checking r from " << low_r << " to " << high_r << endl;

    int higher_thresh = 0;
    unordered_map<circle, int, CircleHasher> cVotes;

    vector<circle> res;

    //Iterate thru all possible r(s) to find the most probable r.
    for (int r = int(low_r); r <= int(high_r); r += 5) {
        cout << '\r' << (double)(r - (int)low_r) / total_r * 100 << "% voting completed." << flush;
        int maxVote = 0;
        //If a position is an edge point and is on the current circle, increment
        // the count of that position.
        cimg_forXY(edges, x, y) {
            if (edges(x, y) == 255) {
                for (int i = 0; i < houghSpaceSlice; i++) {
                    double angle = i * interval;
                    int circleX = int(x - r * cos(angle));
                    int circleY = int(y - r * sin(angle));
                    if (circleX >= 0 && circleX < max_x && circleY >= 0 && circleY < max_y) {
                        if (cVotes.count({circleX, circleY, r})) {
                            cVotes[{circleX, circleY, r}]++;
                        } else {
                            cVotes.insert(make_pair(circle({circleX, circleY, r}), 1));
                        }
                    }
                }
            }
        }
    }

    cout << '\r' << "100% voting completed." << endl;

    //find the maximum
    for (const auto & cv : cVotes) {
        if (cv.second > higher_thresh) {
            higher_thresh = cv.second;
        }
    }

    //Thresh is for eliminating less probable circles
    int thresh = higher_thresh * houghThreshPercent;

    cout << "Hough threshold is " << thresh << endl;

    cout << "Processing " << cVotes.size() << " Voted circles." << endl;
    int processed = 0;

    for (const auto & cv : cVotes) {
        if (cv.second < thresh) continue;
        cout << '\r' << (double)(processed++) / cVotes.size() << "% peocessed." << flush;
        bool addNewCircle = true;
        for (auto & c : res) {
            //Deprecate circles that are close enough
            if (abs(c[0] - cv.first[0]) < 50 &&
                abs(c[1] - cv.first[1]) < 50 &&
                abs(c[2] - cv.first[2]) < 50) {
                addNewCircle = false;
                if (cVotes[c] < cv.second) {
                    c = cv.first;
                }
            }
        }
        if (addNewCircle) {
            res.push_back(cv.first);
        }
    }
    cout << '\r' << "100% peocessed." << endl;
    return res;
}

/*****Draw all the input circles on the input image****************/
void CoinDetect::drawCircle(const vector<CoinDetect::circle> &circles, CImg<unsigned char> &canvas) {
    const unsigned char colorRed[] = {255, 0, 0};
    for (auto c : circles) {
        //Draw outline
        for (int i = -5; i < 6; i++) {
            canvas.draw_circle(c[0], c[1], c[2] + i, colorRed, 1, 0);
        }
    }
}



