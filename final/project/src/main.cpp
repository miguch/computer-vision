#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>
#include "OpenCV_utils.h"
#include "segmentation/segmentTools.h"
#include "utils.h"
#include "adaboost/adaboostTools.h"
#include "a4/a4Tools.h"

using namespace cimg_library;
using namespace std;

void getIndexedA4(shared_ptr<CImg<unsigned char>> src, vector<CImg<unsigned char>> &imgList,
                  int index) {
    auto a4 = a4Tools::adjustA4Image(*src, to_string(index).c_str());
    imgList.at(index) = a4;
}

void getThreshImg(const CImg<unsigned char> &src, vector<CImg<unsigned char>> &imgList,
                  int index) {
    const double thresh = 0.12;
    auto grey = utils::toGreyScale(src);
    auto threshed = utils::reverseAdaptiveThreshold(grey, thresh);
    utils::removeEdges(threshed);
    imgList.at(index) = threshed;
}


void getImgSegments(const CImg<unsigned char> &src,
                    vector<vector<vector<CImg<unsigned char>>>> &imgList,
                    int index) {
    auto segs = segmentTools::getSegments(src);
    imgList[index] = segs;
}

void getImgPredict(const CImg<unsigned char> &src,
                   vector<int> &digitList,
                   int index, adaboostTools &tools) {
    auto imgData = OpenCV_utils::toTestImage(OpenCV_utils::CImgToCvMat(src));
    digitList.at(index) = tools.predictImg(imgData);
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
        cerr << endl << "Usage: " << argv[0] << " {fileCount | lowIndex highIndex} [skipA4] [skipAdaboost]" << endl;
        return 1;
    }

    int lower = 0, higher;
    bool skipA4 = false, skipAdaboost = false;

    if (argc == 2) {
        higher = atoi(argv[1]);
    } else {
        lower = atoi(argv[1]);
        higher = atoi(argv[2]);
    }

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "skipA4") == 0) {
            skipA4 = true;
        } else if (strcmp(argv[i], "skipAdaboost") == 0) {
            skipAdaboost = true;
        }
    }

    int fileCount = higher - lower;

    unsigned concurrentThreads = std::thread::hardware_concurrency() - 1;
    if (concurrentThreads == 0) {
        concurrentThreads = 1;
    }
    cout << "Using " << concurrentThreads << " thread(s)." << endl;

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

    cout << "Threshold Completed." << endl;

    cout << "Segmenting images..." << endl;

    //Each digits of each rows in each image
    vector<vector<vector<CImg<unsigned char>>>> imgSegments(fileCount, vector<vector<CImg<unsigned char>>>());

    for (int i = lower; i < higher; i++) {

        if (threads.size() == concurrentThreads) {
            threads.front().join();
            threads.pop_front();
        }
        cout << "Segmenting " << i << endl;

        thread worker(getImgSegments, threshedImg[i - lower], ref(imgSegments), i - lower);

        threads.push_back(move(worker));
    }

    for (auto &t : threads) {
        t.join();
    }
    threads.clear();

    //Log the number of rows in files and number of digits in each row,
    // for Python to read.
    ofstream digitsInfo("segmentations/digitsLayout.txt");

    for (int i = lower; i < higher; i++) {
        digitsInfo << imgSegments[i - lower].size() << " ";
        for (int k = 0; k < imgSegments[i - lower].size(); k++) {
            digitsInfo << imgSegments[i - lower][k].size() << " ";
            for (int j = 0; j < imgSegments[i - lower][k].size(); j++) {
                auto filename = pathJoin("segmentations", to_string(i) + "-"
                                                          + to_string(k + 1) + "-"
                                                          + to_string(j + 1) + ".jpg");
                imgSegments[i - lower][k][j].save(filename.c_str());
            }
        }
        digitsInfo << endl;
    }

    digitsInfo.close();

    cout << endl << "Segmentation finished." << endl;

    if (!skipAdaboost) {
        cout << "Using trained adaboost to recognize digits..." << endl;

        //digits predicted by adaboost
        vector<vector<vector<int>>> infos;

        adaboostTools tools;
        for (int i = lower; i < higher; i++) {
            cout << "Processing " << i << endl;
            infos.push_back(vector<vector<int>>());
            for (int k = 0; k < imgSegments[i - lower].size(); k++) {
                infos[i - lower].push_back(vector<int>(imgSegments[i - lower][k].size(), 0));
                for (int j = 0; j < imgSegments[i - lower][k].size(); j++) {
                    imgSegments[i - lower][k][j].resize(28, 28);

                    //Using multithreads somehow causes out of bounds exception here
                    getImgPredict(imgSegments[i - lower][k][j], infos[i - lower][k], j, tools);
                }
            }
        }

        cout << "Saving result..." << endl;

        for (int i = lower; i < higher; i++) {
            for (int k = 0; k < imgSegments[i - lower].size(); k++) {
                for (int j = 0; j < imgSegments[i - lower][k].size(); j++) {
                    stringstream filenameStream;
                    filenameStream << to_string(i) << "-"
                                   << to_string(k + 1) << "-" << to_string(j + 1) << ".jpg";
                    auto filename = pathJoin(pathJoin("AdaboostResult", to_string(infos[i - lower][k][j])),
                                             filenameStream.str());
                    imgSegments[i - lower][k][j].save(filename.c_str());
                }
            }
            utils::saveToCSV(pathJoin(pathJoin("adaboostResult", "xlsx"), to_string(i) + ".csv").c_str(),
                             infos[i - lower]);
        }

    } else {
        cout << "Skip Adaboost." << endl;
    }

    cout << "All operations finished!" << endl;

    return 0;
}
