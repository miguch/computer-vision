#include <iostream>
#include "CImg.h"
#include <string>
#include <memory>
#include <thread>
#include <list>
#include <vector>
#include "a4/tools.h"
#include "histogram/HistoEqual.h"
#include "utils.h"

using namespace cimg_library;
using namespace std;

void getIndexedA4(shared_ptr<CImg<unsigned char>> src, vector<CImg<unsigned char>> &imgList,
                  int index) {
    auto a4 = adjustA4Image(*src, to_string(index).c_str());
    imgList.at(index) = a4;
}

void getThreshImg(const CImg<unsigned char> &src, vector<CImg<unsigned char>> &imgList,
                  int index) {
    const double thresh = 0.13;
    auto grey = utils::toGreyScale(src);
    imgList.at(index) = utils::reverseAdaptiveThreshold(grey, thresh);
}

string pathJoin(string base, string file) {
#ifdef _WIN32
    return base + "\\" + file;
#else
    return base + "/" + file;
#endif
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        cerr << "No file number specified!" << endl;
        cerr << endl << "Usage: " << argv[0] << " {fileCount | lowIndex highIndex}" << endl;
        return 1;
    }

    int lower = 0, higher;
    bool skipA4 = false;

    if (argc == 2) {
        higher = atoi(argv[1]);
    } else {
        lower = atoi(argv[1]);
        higher = atoi(argv[2]);
    }

    if (argc == 4 && strcmp(argv[3], "skipA4") == 0) {
        skipA4 = true;
    }

    int fileCount = higher - lower;

    unsigned concurrentThreads = std::thread::hardware_concurrency() - 1;
    if (concurrentThreads == 0) {
        concurrentThreads = 1;
    }

    list<thread> threads;
    vector<CImg<unsigned char>> a4Images(fileCount, CImg<unsigned char>());

    if (!skipA4) {
        for (int i = lower; i < higher; i++) {
            if (threads.size() == concurrentThreads) {
                threads.front().join();
                threads.pop_front();
            }

            auto filename = pathJoin("testcases", to_string(i) + ".jpg");

            shared_ptr<CImg<unsigned char>> src(new CImg<unsigned char>(filename.c_str()));

            thread worker(getIndexedA4, src, ref(a4Images), i - lower);

            threads.push_back(move(worker));

        }

        for (auto &t : threads) {
            t.join();
        }
        threads.clear();

        for (int i = lower; i < higher; i++) {
            auto filename = pathJoin("a4", to_string(i) + ".jpg");
            a4Images[i - lower].save(filename.c_str());
        }

        cout << endl << "A4 complete." << endl;
    } else {
        cout << "Skip A4." << endl;
        for (int i = lower; i < higher; i++) {
            auto filename = pathJoin("a4", to_string(i) + ".jpg");
            threads.emplace_back([&](int index, string file) {
                a4Images.at(index - lower) = CImg<unsigned char>(file.c_str());
            }, i, filename);
        }

        for (auto &t : threads) {
            t.join();
        }
        threads.clear();
    }

    vector<CImg<unsigned char>> threshedImg(fileCount, CImg<unsigned char>());

    cout << "Applying threshold to images..." << endl;

    for (int i = lower; i < higher; i++) {

        if (threads.size() == concurrentThreads) {
            threads.front().join();
            threads.pop_front();
        }
        cout << "Threshing " << i << endl;

        thread worker(getThreshImg, a4Images[i - lower], ref(threshedImg), i - lower);

        threads.push_back(move(worker));
    }

    for (auto &t : threads) {
        t.join();
    }
    threads.clear();

    for (int i = lower; i < higher; i++) {
        auto filename = pathJoin("thresh", to_string(i) + ".jpg");
        threshedImg.at(i - lower).save(filename.c_str());
    }



    return 0;
}
