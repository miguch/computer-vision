//
// Created by Miguel Chan on 2018/11/29.
//

#include "blending.h"
#include "utils.h"
#include <cmath>
#include <algorithm>
#include <cstdint>

using namespace std;

blending::blending(const CImg<unsigned char> &src, const CImg<unsigned char> &dst, const RANSAC::homography_mat &matAB,
                   const RANSAC::homography_mat &matBA): src(src), dst(dst), matAB(matAB), matBA(matBA) {}

CImg<unsigned char> blending::run() {
    int width = getWarpedWidth();
    int height = getWarpedHeight();

    auto a = warpImage(width, height, getMinX(), getMinY());
    auto b = moveImage(width, height, getMinX(), getMinY());

    a.display();
    b.display();
    return blend(a, b);

}


bool isEmpty(const CImg<unsigned char> &img, int x, int y) {

    return (img(x, y, 0) == 0 && img(x, y, 1) == 0 && img(x, y, 2) == 0);
}

//Use poisson blend to blend two image
CImg<unsigned char> blending::blend(CImg<unsigned char> &a, CImg<unsigned char> &b) {

    // Find the center point of a and overlapping part.
    double sum_a_x = 0;
    double sum_a_y = 0;
    int a_n = 0;
    //double sum_b_x = 0;
    //double sum_b_y = 0;
    //int b_n = 0;
    double sum_overlap_x = 0;
    double sum_overlap_y = 0;
    int overlap_n = 0;
    if (a.width() > a.height()) {
        for (int x = 0; x < a.width(); x++) {
            if (!isEmpty(a, x, a.height() / 2)) {
                sum_a_x += x;
                a_n++;
            }

            //if (!isEmpty(b, x, b.height() / 2)) {
            //	sum_b_x += x;
            //	b_n++;
            //}

            if (!isEmpty(a, x, a.height() / 2) && !isEmpty(b, x, a.height() / 2)) {
                sum_overlap_x += x;
                overlap_n++;
            }
        }
    }
    else {
        for (int y = 0; y < a.height(); y++) {
            if (!isEmpty(a, a.width() / 2, y)) {
                sum_a_y += y;
                a_n++;
            }

            if (!isEmpty(a, a.width() / 2, y) && !isEmpty(b, b.width() / 2, y)) {
                sum_overlap_y += y;
                overlap_n++;
            }
        }
    }

    int min_len = (a.width() < a.height()) ? a.width() : a.height();

    int n_level = floor(log2(min_len));

    vector<CImg<float> > a_pyramid(n_level);
    vector<CImg<float> > b_pyramid(n_level);
    vector<CImg<float> > mask(n_level);

    // Initialize the base.
    a_pyramid[0] = a;
    b_pyramid[0] = b;
    mask[0] = CImg<float>(a.width(), a.height(), 1, 1, 0);

    if (a.width() > a.height()) {
        if (sum_a_x / a_n < sum_overlap_x / overlap_n) {
            for (int x = 0; x < sum_overlap_x / overlap_n; x++) {
                for (int y = 0; y < a.height(); y++) {
                    mask[0](x, y) = 1;
                }
            }
        }
        else {
            for (int x = sum_overlap_x / overlap_n + 1; x < a.width(); x++) {
                for (int y = 0; y < a.height(); y++) {
                    mask[0](x, y) = 1;
                }
            }
        }
    }
    else {
        if (sum_a_y / a_n < sum_overlap_y / overlap_n) {
            for (int x = 0; x < a.width(); x++) {
                for (int y = 0; y < sum_overlap_y / overlap_n; y++) {
                    mask[0](x, y) = 1;
                }
            }
        }
        else {
            for (int x = 0; x < a.width(); x++) {
                for (int y = sum_overlap_y / overlap_n; y < a.height(); y++) {
                    mask[0](x, y) = 1;
                }
            }
        }
    }

    // Down sampling a and b, building Gaussian pyramids.
    for (int i = 1; i < n_level; i++) {
        a_pyramid[i] = a_pyramid[i - 1].get_blur(2).get_resize(a_pyramid[i - 1].width() / 2, a_pyramid[i - 1].height() / 2, 1, a_pyramid[i - 1].spectrum(), 3);
        b_pyramid[i] = b_pyramid[i - 1].get_blur(2).get_resize(b_pyramid[i - 1].width() / 2, b_pyramid[i - 1].height() / 2, 1, b_pyramid[i - 1].spectrum(), 3);

        mask[i] = mask[i - 1].get_blur(2).get_resize(mask[i - 1].width() / 2, mask[i - 1].height() / 2, 1, mask[i - 1].spectrum(), 3);
    }

    // Building Laplacian pyramids.
    for (int i = 0; i < n_level - 1; i++) {
        a_pyramid[i] = a_pyramid[i] - a_pyramid[i + 1].get_resize(a_pyramid[i].width(), a_pyramid[i].height(), 1, a_pyramid[i].spectrum(), 3);
        b_pyramid[i] = b_pyramid[i] - b_pyramid[i + 1].get_resize(b_pyramid[i].width(), b_pyramid[i].height(), 1, b_pyramid[i].spectrum(), 3);
    }

    vector<CImg<float> > blend_pyramid(n_level);

    for (int i = 0; i < n_level; i++) {
        blend_pyramid[i] = CImg<float>(a_pyramid[i].width(), a_pyramid[i].height(), 1, a_pyramid[i].spectrum(), 0);

        for (int x = 0; x < blend_pyramid[i].width(); x++) {
            for (int y = 0; y < blend_pyramid[i].height(); y++) {
                for (int k = 0; k < blend_pyramid[i].spectrum(); k++) {
                    blend_pyramid[i](x, y, k) = a_pyramid[i](x, y, k) * mask[i](x, y) + b_pyramid[i](x, y, k) * (1.0 - mask[i](x, y));

                }
            }
        }
    }

    CImg<float> res = blend_pyramid[n_level - 1];
    for (int i = n_level - 2; i >= 0; i--) {
        res.resize(blend_pyramid[i].width(), blend_pyramid[i].height(), 1, blend_pyramid[i].spectrum(), 3);

        for (int x = 0; x < blend_pyramid[i].width(); x++) {
            for (int y = 0; y < blend_pyramid[i].height(); y++) {
                for (int k = 0; k < blend_pyramid[i].spectrum(); k++) {
                    float t = res(x, y, k) + blend_pyramid[i](x, y, k);

                    if (t > 255) {
                        t = 255;
                    }
                    else if (t < 0) {
                        t = 0;
                    }

                    res(x, y, k) = t;
                }
            }
        }
    }

    return res;
}

int blending::getWarpedHeight() {
    auto ys = {getWarpedY(0, 0, matAB), getWarpedY(0, dst.height() - 1, matAB),
               getWarpedY(dst.width() - 1, 0, matAB), getWarpedY(dst.width() - 1, dst.height() - 1, matAB)};
    double min_y = *min_element(begin(ys), end(ys));
    double max_y = *max_element(begin(ys), end(ys));

    min_y = min_y < 0 ? min_y : 0;
    max_y = max_y > src.height() ? max_y : src.height();
    return max_y - min_y;
}

int blending::getWarpedWidth() {
    auto xs = {getWarpedX(0, 0, matAB), getWarpedX(0, dst.height() - 1, matAB),
               getWarpedX(dst.width() - 1, 0, matAB), getWarpedX(dst.width() - 1, dst.height() - 1, matAB)};
    double min_x = *min_element(begin(xs), end(xs));
    double max_x = *max_element(begin(xs), end(xs));

    min_x = min_x < 0 ? min_x : 0;
    max_x = max_x > src.width() ? max_x : src.width();
    return max_x - min_x;
}

double blending::getWarpedY(double x, double y, const RANSAC::homography_mat& mat) {
    return RANSAC::getHomographyY(mat, x, y);
}

double blending::getWarpedX(double x, double y, const RANSAC::homography_mat& mat) {
    return RANSAC::getHomographyX(mat, x, y);
}

double blending::getMinX() {
    auto xs = {getWarpedX(0, 0, matAB), getWarpedX(0, dst.height() - 1, matAB),
               getWarpedX(dst.width() - 1, 0, matAB), getWarpedX(dst.width() - 1, dst.height() - 1, matAB)};
    double min_x = *min_element(begin(xs), end(xs));

    return min_x < 0 ? min_x : 0;
}

double blending::getMinY() {
    auto ys = {getWarpedY(0, 0, matAB), getWarpedY(0, dst.height() - 1, matAB),
               getWarpedY(dst.width() - 1, 0, matAB), getWarpedY(dst.width() - 1, dst.height() - 1, matAB)};
    double min_y = *min_element(begin(ys), end(ys));

    return min_y < 0 ? min_y : 0;
}

void blending::updateFeatureWithMat(std::map<std::array<float, 128>, VlSiftKeypoint> &feature) {
    float offsetX = getMinX(), offsetY = getMinY();
    for (auto & feat : feature) {
        float x = feat.second.x, y = feat.second.y;
        feat.second.x = getWarpedX(x, y, matAB) + offsetX;
        feat.second.y = getWarpedY(x, y, matAB) + offsetY;
        feat.second.ix = int(feat.second.x);
        feat.second.iy = int(feat.second.y);
    }
}

void blending::updateFeatureWithOffset(std::map<std::array<float, 128>, VlSiftKeypoint> &feature) {
    float offsetX = getMinX(), offsetY = getMinY();
    for (auto & feat : feature) {
        feat.second.x -= offsetX;
        feat.second.y -= offsetY;
        feat.second.ix = int(feat.second.x);
        feat.second.iy = int(feat.second.y);
    }
}

CImg<unsigned char> blending::warpImage(int width, int height, int offset_x, int offset_y) {
    CImg<unsigned char> result(width, height, 1, dst.spectrum(), 0);
    cimg_forXY(result, x, y) {
            int dst_x = getWarpedX(x + offset_y, y + offset_x, matBA);
            int dst_y = getWarpedY(x + offset_y, y + offset_x, matBA);

            if (dst_x >= 0 && dst_x < dst.width() && dst_y >= 0 && dst_y < dst.height()) {
                for (int i = 0; i < dst.spectrum(); i++) {
                    result(x, y, i) = utils::interpolate(dst, x, y, i);
                }
            }
        }
    return result;
}


CImg<unsigned char> blending::moveImage(int width, int height, int offset_y, int offset_x) {
    CImg<unsigned char> result(width, height, 1, src.spectrum(), 0);
    cimg_forXY(result, x, y) {
            int src_x = x + offset_x;
            int src_y = y + offset_y;

            if (src_x >= 0 && src_x < src.width() && src_y >= 0 && src_y < src.height()) {
                for (int i = 0; i < src.spectrum(); i++) {
                    result(x, y, i) = src(src_x, src_y, i);
                }
            }
        }
    return result;
}




