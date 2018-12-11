//
// Created by Miguel Chan on 2018/11/27.
//

#include "utils.h"

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
