#include <iostream>
#include <string>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

class Canny {
private:
    bool c_skipNMS;
    CImg<unsigned char> origin;
    CImg<unsigned char> grey_scale;
    CImg<unsigned char> smooth_img;
    CImg<float> gauss_smooth;
    int c_thresh1, c_thresh2, c_scale;
    float c_sigma;
    void createGreyScale();
    template <class T>
    static void reflect(CImg<T>& target, int nrows, int ncols, int c_scale);
    void gaussSmoothing(float sigma, int scale);
    CImg<unsigned char> nms(CImg<float>& gradient, CImg<float> &dir);
    void hysThres(CImg<unsigned char>& edgeMap, int count);


public:
    Canny(string filename, bool skipNMS, int thresh1, int thresh2, int scale, float sigma);

    CImg<unsigned char> getGreyScale();

    CImg<unsigned char> getSmoothImage();

    CImg<unsigned char> runCanny();

    pair<CImg<float>, CImg<float>> sobel();

    static CImg<unsigned char> connectEdges(CImg<unsigned char> & input);
    static CImg<unsigned char> deleteShortEdges(CImg<unsigned char> & input);
    static CImg<unsigned char> connectAndDelete(CImg<unsigned char> & input);
    
};