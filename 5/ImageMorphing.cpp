//
// Created by Miguel Chan on 2018/11/7.
//

#include "ImageMorphing.h"
#include <iostream>
#include <array>
#include <fstream>
#include <cmath>

ImageMorphing::ImageMorphing(string s, string d) {
    source = CImg<unsigned char>(s.c_str());
    destination = CImg<unsigned char>(d.c_str()).resize(source);
}

pair<CImg<unsigned char>,CImgList<unsigned char>> ImageMorphing::morph(const vector<pair<ImageMorphing::point, ImageMorphing::point>>& points, unsigned intermediate) {
    CImg<unsigned char> res(source);
    CImgList<unsigned char> vid;
    vid.push_back(source);
    double interval = 1.0 / (intermediate+1);
    vector<point> srcOld, destOld;
    for (auto & p : points) {
        srcOld.push_back(p.first);
        destOld.push_back(p.second);
    }
    auto srcLines = makeLines(srcOld), destLines = makeLines(destOld);
    for (int i = 1; i <= intermediate; i++) {
        cout << "\rGenerating Frame " << i << "..." << flush;
        CImg<unsigned char> inter(destination.width(), destination.height(), 1, 3, 0);
        double alpha = interval * i;
        vector<point> interPoints;
        //intermediate points for warping
        for (auto & p : points) {
            interPoints.emplace_back((1.0 - alpha) * p.first.first + alpha * p.second.first,
                                     (1.0 - alpha) * p.first.second + alpha * p.second.second);
        }
        auto interLines = makeLines(interPoints);
        auto newSrc = warp(source, srcLines, interLines);
        auto newDest = warp(destination, destLines, interLines);
        cimg_forXY(inter, x, y) {
            for (int i = 0; i < 3; i++) inter(x, y, i) = (1.0 - alpha) * newSrc(x, y, i) + alpha * newDest(x, y, i);
        }
        res.append(inter);
        vid.push_back(inter);
    }
    cout << endl;
    vid.push_back(destination);
    res.append(destination);
    return {res, vid};
}

//different colors to display selected points.
const unsigned char colors[8][3] = {
        {255, 0, 0}, {255, 255, 0}, {0, 255, 0}, {0, 255, 255},
        {0, 0, 255}, {255, 0, 255}, {153, 153, 0}, {255, 153, 204}
};

//Open a display window for user to choose the feature points, return the points list after user close the window
vector<pair<ImageMorphing::point, ImageMorphing::point>> ImageMorphing::selectPoints(const vector<pair<ImageMorphing::point,
        ImageMorphing::point>>& existing) {
    vector<pair<point, point>> res;
    CImgDisplay disp;
    disp.set_title("Choose feature points");
    auto twoImg = source.get_append(destination);
    twoImg.display(disp);
    cimg::dialog("Choose feature points", "Choose the corresponding feature points in the two images, close the window after the operation.");
    bool choosingLeft = true;
    point leftPoint;
    int colorIndex = 0;
    for (int i = 0; i < existing.size(); i++) {
        twoImg.draw_circle(existing[i].first.first, existing[i].first.second, 5, colors[colorIndex % 8]);
        twoImg.draw_circle(existing[i].second.first + source.width(), existing[i].second.second, 5, colors[(colorIndex++) % 8]);
        if (i % 2) {
            twoImg.draw_line(existing[i].first.first, existing[i].first.second, existing[i-1].first.first, existing[i-1].first.second, colors[(colorIndex-1) % 8]);
            twoImg.draw_line(existing[i].second.first + source.width(), existing[i].second.second, existing[i-1].second.first + destination.width(), existing[i-1].second.second, colors[(colorIndex-1) % 8]);
        }
        res.push_back(existing[i]);
    }
    twoImg.display(disp);
    while (!disp.is_closed()) {
        //left click
        if (disp.button() & 1) {
            point clicked = {disp.mouse_x(), disp.mouse_y()};
            if (clicked.first < source.width()) {
                if (clicked.second > source.height()) {
                    cimg::dialog("Wrong point", "Clicked position is not in the image area");
                } else if (choosingLeft) {
                    choosingLeft = false;
                    leftPoint = clicked;
                    twoImg.draw_circle(clicked.first, clicked.second, 5, colors[colorIndex % 8]);
                    twoImg.display(disp);
                } else {
                    cimg::dialog("Wrong point", "You should now choose the corresponding point from the right image");
                }
            } else {
                if (clicked.second > destination.height()) {
                    cimg::dialog("Wrong point", "Clicked position is not in the image area");
                } else if (!choosingLeft) {
                    choosingLeft = true;
                    point rightPoint = {clicked.first - source.width(), clicked.second};
                    twoImg.draw_circle(clicked.first, clicked.second, 5, colors[(colorIndex++) % 8]);
                    res.emplace_back(leftPoint, rightPoint);
                    if (res.size() % 2 == 0) {
                        int s = res.size();
                        twoImg.draw_line(res[s-1].first.first, res[s-1].first.second, res[s-2].first.first, res[s-2].first.second, colors[(colorIndex-1) % 8]);
                        twoImg.draw_line(res[s-1].second.first + destination.width(), res[s-1].second.second, res[s-2].second.first + destination.width(), res[s-2].second.second, colors[(colorIndex-1) % 8]);
                    }
                    twoImg.display(disp);
                } else {
                    cimg::dialog("Wrong point", "You should first choose a point from the left image");
                }
            }

        }
        disp.wait();
    }
    return res;
}

//Read the points info from the input file
vector<pair<ImageMorphing::point, ImageMorphing::point>> ImageMorphing::readPointsFromFile(string filename) {
    vector<pair<point, point>> res;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open points file: " << filename << endl;
        exit(1);
    }
    int pos[4];
    while (file >> pos[0]) {
        for (int i = 1; i < 4; i++) {
            if (!(file >> pos[i])) {
                cerr << "Input points file is incorrect." << endl;
                exit(1);
            }
        }
        res.emplace_back(point({pos[0], pos[1]}), point({pos[2], pos[3]}));
    }
    return res;
}

//Save the input points info to file
void
ImageMorphing::savePointToFile(const vector<pair<ImageMorphing::point, ImageMorphing::point>>& points, string filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open points file to write: " << filename << endl;
        exit(1);
    }
    for (const auto & p : points) {
        file << p.first.first << " " << p.first.second << " " << p.second.first << " " << p.second.second << endl;
    }
}

//Use Beier & Neely algorithm to warp image
CImg<unsigned char> ImageMorphing::warp(const CImg<unsigned char> &src, vector<pair<ImageMorphing::point, ImageMorphing::point>> oldLines,
                                        vector<pair<ImageMorphing::point, ImageMorphing::point>> newLines) {
    const double weightA = 1, weightB = 1, weightP = 2;
    CImg<unsigned char> res(src.width(), src.height(), 1, 3, 0);
    cimg_forXY(res, x, y) {
        point dSum = {0, 0};
        double weightSum = 0;
        for (int i = 0; i < newLines.size(); i++) {
            point p = newLines[i].first, q = newLines[i].second;
            point sp = oldLines[i].first, sq = oldLines[i].second;
            double distSquare = (pow(q.first - p.first, 2) + pow(q.second - p.second, 2));
            double sDistSquare = (pow(sq.first - sp.first, 2) + pow(sq.second - sp.second, 2));
            double u = ((x - p.first) * (q.first - p.first) + (y - p.second) * (q.second - p.second)) /
                       distSquare;
            double v = ((x - p.first) * (p.second - q.second) + (y - p.second) * (q.first - p.first)) /
                    sqrt(distSquare);
            double newX = sp.first + u * (sq.first - sp.first) + ((v * (sp.second - sq.second)) / sqrt(sDistSquare));
            double newY = sp.second + u * (sq.second - sp.second) + ((v * (sq.first - sp.first)) / sqrt(sDistSquare));
            double displaceX = newX - x, displaceY = newY - y;
            double dist;
            if (u < 0) dist = sqrt(pow(x - p.first, 2) + pow(y - p.second, 2));
            else if (u > 1) dist = sqrt(pow(x - q.first, 2) + pow(y - q.second, 2));
            else dist = fabs(v);
            double weight = pow(pow(sqrt(distSquare), weightP) / (weightA + dist), weightB);
            dSum.first += displaceX * weight;
            dSum.second += displaceY * weight;
            weightSum += weight;
        }
        int srcX = x, srcY = y;
        if (weightSum != 0) {
            srcX += dSum.first / weightSum, srcY += dSum.second / weightSum;
        }
        srcX = max(0, min(srcX, src.width() - 1));
        srcY = max(0, min(srcY, src.height() - 1));
        for (int k = 0; k < 3; k++) res(x, y, k) = src(srcX, srcY, k);
    }
    //res.display();
    return res;
}

//make a list of start and end points of lines from the input point vector.
vector<pair<ImageMorphing::point, ImageMorphing::point>>
ImageMorphing::makeLines(const vector<ImageMorphing::point>& points) {
    vector<pair<point, point>> res;
    for (int i = 0; i < points.size() / 2; i++) {
        res.emplace_back(points[i * 2], points[i * 2 + 1]);
    }
    return res;
}





