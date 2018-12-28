//
// Created by Miguel Chan on 2018/12/25.
//

#include "segmentation.h"
#include <iostream>
#include <thread>
#include <stack>
#include <algorithm>
#include "src/utils.h"
#include <cmath>

using namespace std;

segmentation::segmentation(const CImg<unsigned char> &src): src(src) {

}

std::vector<int> segmentation::getRowsIntensity(const CImg<unsigned char>& img) {

    vector<int> result(img.height(), 0);

    cimg_forY(img, y) {
        cimg_forX(img, x) {
            if (img(x, y) == 255) {
                ++result[y];
            }
        }
    }

    return result;
}

std::vector<int> segmentation::getColsIntensity(const CImg<unsigned char>& img, int y1, int y2) {
    vector<int> result(img.width(), 0);

    cimg_forX(img, x) {
        for (int i = y1; i <= y2; i++) {
            if (img(x, i) == 255) {
                ++result[x];
            }
        }
    }

    return result;
}

std::vector<vector<CImg<unsigned char>>> segmentation::runSegmentation() {
    vector<vector<CImg<unsigned char>>> result;

    CImg<unsigned char> r(src.width(), src.height(), 1, 1, 0);

    auto rInt = getRowsIntensity(src);

    auto rSegments = getRowSegments(src, rInt);

    for (int i = 0; i < rSegments.size(); i++) {
        CImg<unsigned char> seg(src.width(), rSegments[i].second - rSegments[i].first, 1, 1, 0);
        cimg_forXY(seg, x, y) {
            seg(x, y) = src(x, y + rSegments[i].first);
        }
        vector<CImg<unsigned char>> row;
        auto cInt = getColsIntensity(seg, 0, seg.height() - 1);
        auto cSegments = getColSegments(seg, cInt);
        for (int i = 0; i < cSegments.size(); i++) {
            CImg<unsigned char> digit(cSegments[i].second - cSegments[i].first, seg.height(), 1, 1, 0);
            cimg_forXY(digit, x, y) {
                    digit(x, y) = seg(x + cSegments[i].first, y);
            }
            row.push_back(digit);
        }
        auto digits = regionSegment(row);
        result.push_back(digits);
    }

    return result;
}

std::vector<std::pair<int, int>> segmentation::getRowSegments(const CImg<unsigned char>& img, const std::vector<int> &rows) {
    vector<pair<int, int>> result;

    int start = -1;

    for (int i = 0; i < rows.size(); i++) {
        if (rows[i] != 0) {
            if (start == -1) {
                start = i;
            }
        } else {
            if (start != -1) {
                if (i - start > 15) {
                    int low = max(0, start - 4);
                    int high = min(int(rows.size() - 1), i + 4);
                    result.emplace_back(low, high);
                }
                start = -1;
            }
        }
    }
    if (start != -1) {
        result.emplace_back(start, rows.size() - 1);
    }


    return result;
}

std::vector<std::pair<int, int>>
segmentation::getColSegments(const CImg<unsigned char> &img, const std::vector<int> &cols) {
    vector<pair<int, int>> result;

    int start = -1;

    for (int i = 0; i < cols.size(); i++) {
        if (cols[i] != 0) {
            if (start == -1) {
                start = i;
            }
        } else {
            if (start != -1) {
                if (i - start > 2) {
                    int low = max(0, start - 2);
                    int high = min(int(cols.size() - 1), i + 2);
                    result.emplace_back(low, high);
                }
                start = -1;
            }
        }
    }
    if (start != -1) {
        result.emplace_back(start, cols.size() - 1);
    }


    return result;
}

vector<CImg<unsigned char>> regionGrow(const CImg<unsigned char>& origin) {
    typedef pair<int, int> pt;
    vector<CImg<unsigned char>> result;
    CImg<bool> used(origin.width(), origin.height(), 1, 1, false);
    cimg_forY(origin, y) cimg_forX(origin, x) {
        stack<pt> seeds;
        int minX = origin.width(), maxX = -1;
        int minY = origin.height(), maxY = -1;
        if (origin(x, y) == 255 && !used(x, y)) {
            seeds.push({x, y});
        }
        while (!seeds.empty()) {
            auto curr = seeds.top();
            seeds.pop();
            if (curr.first > maxX) {
                maxX = curr.first;
            }
            if (curr.first < minX) {
                minX = curr.first;
            }
            if (curr.second > maxY) {
                maxY = curr.second;
            }
            if (curr.second < minY) {
                minY = curr.second;
            }
            for (int i = -2; i <= 2; i++) {
                for (int k = -2; k <= 2; k++) {
                    int nx = curr.first + i;
                    int ny = curr.second + k;
                    nx = max(0, min(origin.width() - 1, nx));
                    ny = max(0, min(origin.height() - 1, ny));
                    if (origin(nx, ny) == 255 && !used(nx, ny)) {
                        seeds.push({nx, ny});
                        used(nx, ny) = true;
                    }
                }
            }
        }
        if (maxY - minY > 15) {
            CImg<unsigned char> digit(maxX - minX + 6, maxY - minY + 6, 1, 1, 0);
            for (int i = minX; i <= maxX; i++) {
                for (int k = minY; k <= maxY; k++) {
                    digit(i - minX + 3, k - minY + 3) = origin(i, k);
                }
            }
            result.push_back(move(digit));
        }
    }
    return result;
}


//Deal with multiple digits in one segments, using region grow
std::vector<CImg<unsigned char>> segmentation::regionSegment(const std::vector<CImg<unsigned char>> &originSegs) {
    vector<CImg<unsigned char>> result;

    for (const auto &img: originSegs) {
        auto actual = regionGrow(img);
        for (auto &digit: actual) {
            result.push_back(utils::toSquareImage(digit));
        }
    }

    return result;
}




