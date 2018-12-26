//
// Created by Miguel Chan on 2018/11/27.
//

#include "utils.h"
#include <cmath>
#include <iostream>
#include <stack>
#include <fstream>
#include <vector>

unsigned char utils::interpolate(const CImg<unsigned char> &src, double x, double y, int channel) {
    int x1 = int(x), y1 = int(y);
    double x1_diff = x - x1, y1_diff = y - y1;
    double x2_diff = 1 - x1_diff, y2_diff = 1 - y1_diff;
    int x2 = (x1 >= src.width() - 1) ? x1 : x1 + 1;
    int y2 = (y1 >= src.height() - 1) ? y1 : y1 + 1;

    double fxy1 = x2_diff * src(x1, y1, channel) + x1_diff * src(x2, y1, channel);
    double fxy2 = x2_diff * src(x1, y2, channel) + x1_diff * src(x2, y2, channel);

    return (unsigned char) (y2_diff * fxy1 + y1_diff * fxy2);
}


CImg<unsigned char> utils::toGreyScale(const CImg<unsigned char> &src) {
    CImg<unsigned char> result(src.width(), src.height());
    cimg_forXY(src, x, y) {
            auto r = src(x, y, 0), g = src(x, y, 1), b = src(x, y, 2);
            result(x, y) = 0.299 * r + 0.587 * g + 0.114 * b;
        }
    return result;
}

unsigned char utils::getGreyPixel(unsigned char r, unsigned char g, unsigned char b) {
    return 0.299 * r + 0.587 * g + 0.114 * b;
}

CImg<double> utils::RGBtoLab(const CImg<unsigned char> src) {
    //RGB to LMS
    CImg<double> lms(src.width(), src.height(), 1, 3, 0);
    cimg_forXY(src, x, y) {
            float r = src(x, y, 0), g = src(x, y, 1), b = src(x, y, 2);
            lms(x, y, 0) = 0.3811 * r + 0.5783 * g + 0.0402 * b; // l
            lms(x, y, 1) = 0.1967 * r + 0.7244 * g + 0.0782 * b; // m
            lms(x, y, 2) = 0.0241 * r + 0.1288 * g + 0.8444 * b; // s
        }
    //LMS to lab
    CImg<double> lab(src.width(), src.height(), 1, 3, 0);
    cimg_forXY(lms, x, y) {

            double L = lms(x, y, 0) == 0 ? 0 : log10(lms(x, y, 0)),
                    M = lms(x, y, 1) == 0 ? 0 : log10(lms(x, y, 1)),
                    S = lms(x, y, 2) == 0 ? 0 : log10(lms(x, y, 2));
            lab(x, y, 0) = (L + M + S) / sqrt(3); //L
            lab(x, y, 1) = (L + M - 2 * S) / sqrt(6); //a
            lab(x, y, 2) = (L - M) / sqrt(2); //b
        }
    return lab;
}


CImg<unsigned char> utils::LabtoRGB(const CImg<double> src) {
    CImg<double> lms(src.width(), src.height(), 1, 3, 0);
    cimg_forXY(src, x, y) {
            double l = src(x, y, 0) / sqrt(3), a = src(x, y, 1) / sqrt(6), b = src(x, y, 2) / sqrt(2);
            lms(x, y, 0) = l + a + b;
            lms(x, y, 1) = l + a - b;
            lms(x, y, 2) = l - a * 2;
        }
    //LMS to RGB
    CImg<unsigned char> rgb(src.width(), src.height(), 1, 3, 0);
    cimg_forXY(lms, x, y) {
            double L = pow(10, lms(x, y, 0)), M = pow(10, lms(x, y, 1)), S = pow(10, lms(x, y, 2));
            double r = 4.4679 * L - 3.5873 * M + 0.1193 * S; // l
            double g = -1.2186 * L + 2.3809 * M - 0.1624 * S; // m
            double b = 0.0497 * L - 0.2439 * M + 1.2045 * S; // s
            rgb(x, y, 0) = (unsigned char) (r > 255 ? 255 : (r < 0 ? 0 : r));
            rgb(x, y, 1) = (unsigned char) (g > 255 ? 255 : (g < 0 ? 0 : g));
            rgb(x, y, 2) = (unsigned char) (b > 255 ? 255 : (b < 0 ? 0 : b));
        }
    return rgb;
}

std::array<double, 3> utils::RGBtoLabPix(unsigned char r, unsigned char g, unsigned char b) {
    double l = 0.3811 * r + 0.5783 * g + 0.0402 * b; // l
    double m = 0.1967 * r + 0.7244 * g + 0.0782 * b; // m
    double s = 0.0241 * r + 0.1288 * g + 0.8444 * b; // s
    double L = l == 0 ? 0 : log10(l),
            M = m == 0 ? 0 : log10(m),
            S = s == 0 ? 0 : log10(s);
    double L1 = (L + M + S) / sqrt(3); //L
    double a = (L + M - 2 * S) / sqrt(6); //a
    double b1 = (L - M) / sqrt(2); //b
    return {L1, a, b1};
}

std::array<unsigned char, 3> utils::LabtoRGBPix(double l, double a, double b) {
    double l1 = l + a + b;
    double m = l + a - b;
    double s = l - a * 2;
    //LMS to RGB
    double L = pow(10, l1), M = pow(10, m), S = pow(10, a);
    double r = 4.4679 * L - 3.5873 * M + 0.1193 * S; // l
    double g = -1.2186 * L + 2.3809 * M - 0.1624 * S; // m
    double b1 = 0.0497 * L - 0.2439 * M + 1.2045 * S; // s
    unsigned char R = (unsigned char) (r > 255 ? 255 : (r < 0 ? 0 : r));
    unsigned char G = (unsigned char) (g > 255 ? 255 : (g < 0 ? 0 : g));
    unsigned char B = (unsigned char) (b1 > 255 ? 255 : (b1 < 0 ? 0 : b1));
    return {R, G, B};
}


//Convert RGB color space to YCbCr color space
CImg<unsigned char> utils::RGBtoYCbCr(const CImg<unsigned char>& img) {
    CImg<unsigned char> ycbcr(img.width(), img.height(), 1, 3, 0);
    cimg_forXY(img, x, y) {
            auto r = img(x, y, 0), g = img(x, y, 1), b = img(x, y, 2);
            int Y = 0.299 * r + 0.587 * g + 0.114 * b; // Y
            int Cb = 128 - 0.168736 * r - 0.331264 * g + 0.5 * b; //CR
            int Cr = 128 + 0.5 * r - 0.418688 * g - 0.081312 * b; //CB
            ycbcr(x, y, 0) = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
            ycbcr(x, y, 1) = (unsigned char)(Cb > 255 ? 255 : (Cb < 0 ? 0 : Cb));
            ycbcr(x, y, 2) = (unsigned char)(Cr > 255 ? 255 : (Cr < 0 ? 0 : Cr));
        }
    return ycbcr;
}

//Convert YCbCr color space to RGB
CImg<unsigned char> utils::YCbCrtoRGB(const CImg<unsigned char> &img) {
    //lab to LMS
    CImg<double> rgb(img.width(), img.height(), 1, 3, 0);
    cimg_forXY(img, x, y) {
            auto Y = img(x, y, 0), cB = img(x, y, 1), cR = img(x, y, 2);
            int r = Y + 1.402 * (cR - 128);
            int g = Y - 0.344136 * (cB - 128) - 0.714136 * (cR - 128);
            int b = Y + 1.772 * (cB - 128);
            rgb(x, y, 0) = (unsigned char)(r > 255 ? 255 : (r < 0 ? 0 : r));
            rgb(x, y, 1) = (unsigned char)(g > 255 ? 255 : (g < 0 ? 0 : g));
            rgb(x, y, 2) = (unsigned char)(b > 255 ? 255 : (b < 0 ? 0 : b));
        }

    return rgb;
}

utils::point utils::RGBtoYCbCrPix(const utils::point &pix) {
    auto r = pix[0], g = pix[1], b = pix[2];
    int Y = 0.299 * r + 0.587 * g + 0.114 * b; // Y
    int Cb = 128 - 0.168736 * r - 0.331264 * g + 0.5 * b; //CR
    int Cr = 128 + 0.5 * r - 0.418688 * g - 0.081312 * b; //CB
    Y = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
    Cb = (unsigned char)(Cb > 255 ? 255 : (Cb < 0 ? 0 : Cb));
    Cr = (unsigned char)(Cr > 255 ? 255 : (Cr < 0 ? 0 : Cr));
    return {static_cast<unsigned char>(Y), static_cast<unsigned char>(Cb), static_cast<unsigned char>(Cr)};
}

utils::point utils::YCbCrtoRGBPix(const utils::point &pix) {
    auto Y = pix[0], cB = pix[1], cR = pix[2];
    int r = Y + 1.402 * (cR - 128);
    int g = Y - 0.344136 * (cB - 128) - 0.714136 * (cR - 128);
    int b = Y + 1.772 * (cB - 128);
    r = (unsigned char)(r > 255 ? 255 : (r < 0 ? 0 : r));
    g = (unsigned char)(g > 255 ? 255 : (g < 0 ? 0 : g));
    b = (unsigned char)(b > 255 ? 255 : (b < 0 ? 0 : b));
    return {static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b)};
}

//Mark all points whose neighbor is not all valid points
CImg<unsigned char> utils::getOutline(const CImg<unsigned char> &src) {
    CImg<unsigned char> result(src.width(), src.height(), 1, 1, 0);

    cimg_forXY(src, x, y) {
            if (src(x, y) != 255) continue;
            int count = 0;
            for (int i = -1; i <= 1; i++) {
                for (int k = -1; k <= 1; k++) {
                    int newX = x + i;
                    int newY = y + k;
                    if (newX < 0 || newX >= src.width() || newY < 0 || newY >= src.height())
                        continue;
                    if (src(newX, newY) == 255)
                        count++;
                }
            }
            if (count != 9) {
                result(x, y) = 255;
            }
    }
    return result;
}

//Change every pixels to 0 or 255 based on thresh
CImg<unsigned char> utils::imgThreshold(const CImg<unsigned char> &img, unsigned char thresh) {
    CImg<unsigned char> result(img.width(), img.height(), 1, 1, 0);

    cimg_forXY(img, x, y) {
        if (img(x, y) >= thresh) {
            result(x, y) = 255;
        }
    }
    return result;
}

//Like imgThreshold, but 0 for above thresh, 255 for lower than thresh
CImg<unsigned char> utils::reverseImgThreshold(const CImg<unsigned char> &img,
                                               unsigned char thresh) {
    CImg<unsigned char> result(img.width(), img.height(), 1, 1, 255);

    cimg_forXY(img, x, y) {
            if (img(x, y) >= thresh) {
                result(x, y) = 0;
            }
        }
    return result;
}

CImg<int> utils::getIntegralImage(const CImg<unsigned char> &img) {
    CImg<int> result(img.width(), img.height(), 1, 1, 0);
    result(0, 0) = img(0, 0);

    for (int i = 1; i < img.width(); i++) {
        result(i, 0) = img(i, 0) + result(i-1, 0);
    }
    for (int i = 1; i < img.height(); i++) {
        result(0, i) = img(0, i) + result(0, i-1);
    }

    for (int y = 1; y < img.height(); y++) {
        for (int x = 1; x < img.width(); x++) {
            result(x, y) = result(x-1, y) + result(x, y-1) - result(x-1, y-1) + img(x, y);
        }
    }

    return result;
}

CImg<unsigned char> utils::adaptiveThreshold(const CImg<unsigned char> &img, double thresh) {
    auto integral = getIntegralImage(img);
    int s = std::max(img.height(), img.width()) / 8;
    CImg<unsigned char> result(img.width(), img.height(), 1, 1, 255);

    int windowRadius = s / 2;

    cimg_forY(img, y) {
        int y1 = std::max(0, y - windowRadius);
        int y2 = std::min(img.height() - 1, y + windowRadius);
        cimg_forX(img, x) {
            int x1 = std::max(0, x - windowRadius);
            int x2 = std::min(img.width() - 1, x + windowRadius);
            int count = (x2 - x1) * (y2 - y1);
            int windowSum = integral(x2, y2) - integral(x1, y2) -
                    integral(x2, y1) + integral(x1, y1);
            if (img(x, y) * count < windowSum * (1.0 - thresh)) {
                result(x, y) = 0;
            }
        }
    }
    return result;

}

CImg<unsigned char> utils::reverseAdaptiveThreshold(const CImg<unsigned char> &img, double thresh) {
    auto integral = getIntegralImage(img);
    int s = std::max(img.height(), img.width()) / 8;
    CImg<unsigned char> result(img.width(), img.height(), 1, 1, 0);

    int windowRadius = s / 2;

    cimg_forY(img, y) {
        int y1 = std::max(0, y - windowRadius);
        int y2 = std::min(img.height() - 1, y + windowRadius);
        cimg_forX(img, x) {
            int x1 = std::max(0, x - windowRadius);
            int x2 = std::min(img.width() - 1, x + windowRadius);
            int count = (x2 - x1) * (y2 - y1);
            int windowSum = integral(x2, y2) - integral(x1, y2) -
                            integral(x2, y1) + integral(x1, y1);
            if (img(x, y) * count < windowSum * (1.0 - thresh)) {
                result(x, y) = 255;
            }
        }
    }
    return result;
}

void removePointsByNeighbor(CImg<unsigned char> &img,
                            std::stack<std::pair<int, int>>& seeds,
                            CImg<bool>& used) {

    while (!seeds.empty()) {
        auto curr = seeds.top();
        seeds.pop();
        if (img(curr.first, curr.second) != 255) continue;
        img(curr.first, curr.second) = 0;
        for (int i = -1; i <= 1; i++) {
            for (int k = -1; k <= 1; k++) {
                if (i == 0 && k == 0) continue;
                int nx = curr.first + i;
                int ny = curr.second + k;
                nx = std::min(img.width() - 1, std::max(nx, 0));
                ny = std::min(img.height() - 1, std::max(ny, 0));
                if (used(nx, ny)) continue;
                if (img(nx, ny) == 255) {
                    seeds.push({nx, ny});
                }
                used(nx, ny) = true;
            }
        }
    }

}

//Remove edges bright bar produced in reverse threshold due to
//not perfect a4 adjustment.
void utils::removeEdges(CImg<unsigned char> &img) {
    CImg<unsigned char> result(img.width(), img.height(), 1, 1, 0);
    CImg<bool> used(img.width(), img.height(), 1, 1, false);

    typedef std::pair<int, int> pt;
    std::vector<int> xs = {0, img.width() - 1};
    std::vector<int> ys = {0, img.height() - 1};

    for (int x : xs) {
        for (int y = 0; y < img.height(); y++) {
            if (img(x, y) == 255) {
                std::stack<pt> seeds;
                seeds.push({x, y});
                removePointsByNeighbor(img, seeds, used);
            }
        }
    }

    for (int y : ys) {
        for (int x = 0; x < img.width(); x++) {
            if (img(x, y) == 255) {
                std::stack<pt> seeds;
                seeds.push({x, y});
                removePointsByNeighbor(img, seeds, used);
            }
        }
    }

}


//Add Black area to shorter coordinate to turn image to square so that
// it will not change shape in classification
CImg<unsigned char> utils::toSquareImage(const CImg<unsigned char> &src) {
    int baseX = 0, baseY = 0;
    if (src.width() > src.height()) {
        baseY = src.width() - src.height();
    } else if (src.width() < src.height()) {
        baseX = src.height() - src.width();
    }
    CImg<unsigned char> result(src.width() + baseX, src.height() + baseY, 1, 1, 0);
    baseX /= 2;
    baseY /= 2;
    cimg_forXY(src, x, y) {
        result(x + baseX, y + baseY) = src(x, y);
    }
    return result;
}

void utils::saveToCSV(const char *filename, const std::vector<std::vector<int>> &list) {
    std::ofstream fs(filename);

    for (const auto &l: list) {
        for (int i = 0; i < l.size(); i++) {
            fs << l[i];
            if (i != l.size() - 1) {
                fs << ',';
            }
        }
        fs << std::endl;
    }
}
