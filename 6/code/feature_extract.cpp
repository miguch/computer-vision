//
// Created by Miguel Chan on 2018/11/26.
//

#include "feature_extract.h"
#include <cmath>
#include "point.h"

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif

const int octaves = 4, blurs = 3;

std::vector<CImg<uint8_t>> feature_extract::getOctaves() {
    if (!scales.empty()) return scales;
    const double sigma0 = sqrt(2) / 2;
    //anti-aliasing the first
    scales.push_back(src.get_resize(src.width() * 2, src.height() * 2).get_blur(0.5, true, true));
    for (int i = 0; i < octaves; i++) {
        double sigma = sigma0;
        for (int j = 0; j < blurs+2; j++) {
            sigma = sigma * pow(2, j/2);
            scales.push_back(scales[scales.size()-1].get_blur(sigma, true, true));
            DOGs.push_back(scales[scales.size()-2] - scales[scales.size()-1]);
        }
        if (i != octaves - 1) {
            //Down sample the third of this octave to the first in the next octave
            const CImg<uint8_t> &octFirst = scales[i * (blurs + 3) + 2];
            scales.push_back(octFirst.get_resize(octFirst.width() / 2, octFirst.height() / 2));
        }
    }
    return scales;
}

std::vector<CImg<int>> feature_extract::getDOGs() {
    if (!DOGs.empty()) return DOGs;
    getOctaves();
    return DOGs;
}

feature_extract::feature_extract(const CImg<uint8_t> &src) : src(src) {}

void feature_extract::findDogsExtrema() {
    for (int i = 0; i < octaves; i++) {
        for (int j = 1; j < blurs + 1; j++) {
            const auto& curr = DOGs[i * (blurs+2) + j];
            const auto& down = DOGs[i * (blurs+2) + j - 1];
            const auto& up = DOGs[i * (blurs+2) + j + 1];

            CImg<uint8_t> maxima(curr.width(), curr.height(), 1, 1, 0);
            CImg<uint8_t> minima(curr.width(), curr.height(), 1, 1, 0);
            CImg<uint8_t> extrema(curr.width(), curr.height(), 1, 1, 0);

            for (int x = 1; x < curr.width() - 1; x++) {
                for (int y = 1; y < curr.height() - 1; y++) {
                    bool isMaxima = true, isMinima = true;
                    for (int nx = -1; nx <= 1; nx++) {
                        for (int ny = -1; ny <= 1; ny++) {
                            if (((nx != 0 || ny != 0) && (curr(x, y) <= curr(x+nx, y+ny))) ||
                                (curr(x, y) <= up(x+nx, y+ny)) ||
                                (curr(x, y) <= down(x+nx, y+ny))) {
                                isMaxima = false;
                            }
                            if (((nx != 0 || ny != 0) && (curr(x, y) >= curr(x+nx, y+ny))) ||
                                (curr(x, y) >= up(x+nx, y+ny)) ||
                                (curr(x, y) >= down(x+nx, y+ny))) {
                                isMinima = false;
                            }
                        }
                    }
                    if (isMaxima)
                        maxima(x, y) = 255;
                    if (isMinima)
                        minima(x, y) = 255;

                }
            }
            extrema = maxima | minima;

            extremas.push_back(extrema);
        }
    }
}

std::vector<CImg<uint8_t>> feature_extract::getExtremas() {
    if (extremas.empty()) {
        findDogsExtrema();
        filterExtrema();
    }
    return extremas;
}

const double CONTRAST_THRESHOLD = 0.03;
const double R_CURVATURE = 10.0;
const double CURV_THRESHOLD = pow((R_CURVATURE+1), 2) / R_CURVATURE;

//Get rid of points with low contrast.
void feature_extract::filterExtrema() {

    for (int i = 0; i < octaves; i++) {
        for (int j = 0; j < blurs; j++) {
            auto & points = extremas[i * blurs + j];
            std::vector<point> locs;
            const auto & current = DOGs[i * (blurs + 2) + j + 1];

            //Find positions
            cimg_forXY(points, x, y) {
                if (points(x, y) != 0)
                    locs.emplace_back(x, y);
            }

            for (auto p : locs) {

                if (abs(current(p.x+1, p.y+1)) < CONTRAST_THRESHOLD) {
                    //Discard low contrast
                    points(p.x, p.y) = 0;
                } else {
                    //Discard extremas on edges.
                    int rx = p.x + 1;
                    int ry = p.y + 1;
                    int fxx = current(rx - 1, ry) + current(rx + 1, ry) - 2 * current(rx, ry);
                    int fyy = current(rx, ry - 1) + current(rx, ry + 1) - 2 * current(rx, ry);
                    int fxy = current(rx - 1, ry - 1) + current(rx + 1, ry + 1) - current(rx - 1, ry + 1) - current(rx + 1, ry - 1);
                    int trace = fxx + fyy;
                    double deter = fxx * fyy - pow(fxy, 2);
                    double curvature = pow(trace, 2) / deter;
                    if (deter == 0 || curvature >= CURV_THRESHOLD) {
                        points(p.x, p.y) = 0;
                    }
                }
            }
        }
    }
}

void feature_extract::assignOrientation() {
    for (int i = 0; i < octaves; i++) {
        for (int j = 0; j < blurs; j++) {
            auto & current = scales[i * (blurs + 3) + j + 1];
            CImg<double> mag(current.width(), current.height());
            CImg<double> orientation(current.width(), current.height());

            for (int x = 1; x < current.width() - 1; x++) {
                for (int y = 1; y < current.height() - 1; y++) {
                    double dx = current(x + 1, y) - current(x - 1, y);
                    double dy = current(x, y + 1) - current(x, y - 1);

                    mag(x, y) = sqrt(dx * dx + dy * dy);
                    orientation(x, y) = atan2(dy, dx);
                }
            }

            magnitudes.push_back(mag);
            orients.push_back(orientation);
        }
    }

}

std::vector<CImg<double>> feature_extract::getMagnitudes() {
    if (magnitudes.empty())
        assignOrientation();
    return magnitudes;
}

std::vector<CImg<double>> feature_extract::getOrientations() {
    if (orients.empty())
        assignOrientation();
    return orients;
}


