//
// Created by Miguel Chan on 2018/11/27.
//

#include "feature_extraction.h"
using namespace std;

const int octaves = 4, levels = 2, o_min = 0;

feature_extraction::feature_extraction(const CImg<unsigned char> &src): src(src) {}

vl_sift_pix * feature_extraction::cimgToData() {
    vl_sift_pix* data = new vl_sift_pix[src.size()];
    for (int i = 0, y = 0; y < src.height(); y++) {
        for (int x = 0; x < src.width(); x++, i++) {
            data[i] = src(x, y);
        }
    }
    return data;
}

map<array<float, 128>, VlSiftKeypoint> feature_extraction::run_extraction() {
    std::map<std::array<float, 128>, VlSiftKeypoint> result;

    auto data = cimgToData();
    auto filter = vl_sift_new(src.width(), src.height(), octaves, levels, o_min);

    if (vl_sift_process_first_octave(filter, data) != VL_ERR_EOF) {
        while (true) {
            vl_sift_detect(filter);
            auto keyPointptr = filter->keys;
            for (int i = 0; i < filter->nkeys; i++) {
                auto keyPoint = *(keyPointptr);
                ++keyPointptr;

                double angles[4];
                int angleCount = vl_sift_calc_keypoint_orientations(filter, angles, &keyPoint);

                for (int j = 0; j < angleCount; j++) {
                    array<vl_sift_pix, 128> descriptor{};
                    vl_sift_calc_keypoint_descriptor(filter, descriptor.data(), &keyPoint, angles[j]);


                    result.emplace(descriptor, keyPoint);
                }
            }
            if (vl_sift_process_next_octave(filter) == VL_ERR_EOF) {
                break;
            }
        }
    }

    vl_sift_delete(filter);

    delete[]data;

    return result;
}



