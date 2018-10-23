#include "canny.h"
#include <unordered_set>
#include <vector>
#include <queue>
#include <set>

//Disable printf
#define printf(fmt, ...)

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

/*Image starts from (-scale, -scale) */
#define scale1XY(img, x, y) img(x + c_scale, y + c_scale)
/*Image starts from (-2*scale, -2*scale) */
#define scale2XY(img, x, y) img(x + 2 * scale, y + 2 * scale)
/*Image starts from (-1, -1) */
#define p1XY(img, x, y) img(x+1, y+1)


/* Constructor for Canny */
Canny::Canny(string filename, bool skipNMS, int thresh1, int thresh2, int scale, float sigma) : origin(
        filename.c_str()) {
    c_skipNMS = skipNMS;
    c_thresh1 = thresh1;
    c_thresh2 = thresh2;
    c_scale = scale;
    c_sigma = sigma;
    createGreyScale();
    gaussSmoothing(c_sigma, c_scale);
}

/* CREATE GREY SCALE */

/*Create a grey scale image from the input image */
/* return the grey scale image */
void Canny::createGreyScale() {
    grey_scale = CImg<unsigned char>(origin.width() + 2 * c_scale, origin.height() + 2 * c_scale);
    cimg_forXY(origin, x, y) {
            auto r = origin(x, y, 0), g = origin(x, y, 1), b = origin(x, y, 2);
            scale1XY(grey_scale, x, y) = 0.299 * r + 0.587 * g + 0.114 * b;
        }
    reflect(grey_scale, origin.height(), origin.width(), c_scale);
    printf("-------------------------------------------------\n");
    printf("Finished processing: Reflected image by %d pixel\n", c_scale);
}

/* GAUSSIAN SMOOTHING */

/* Smooth the grey scale image by a gaussian kernel which depends on the sigma */
/* User driven sigma */
/* Store the result in class member smooth_img */
void Canny::gaussSmoothing(float sigma, int scale) {
    printf("----------------------------------------------------------------\n");
    printf(" Performing Gaussian Smoothing using sigma of %f \n", sigma);

    /* DEFINE A GAUSSIAN FILTER KERNEL */

    printf(" DEFINE A GAUSSIAN KERNEL\n");

    printf(" ALLOCATE MEMORY\n");
    int grows = 4 * scale + 1, gcols = 4 * scale + 1,
            nrows = origin.height(), ncols = origin.width();
    CImg<float> gauss_filter(gcols, grows);
    printf("ALLOCATED MEMORY\n");

    int set = -scale, i, j, c, r;
    printf(" ALLOCATE MEMORY FOR GAUSS SMOOTH\n");
    gauss_smooth = CImg<float>(ncols + 4 * scale, nrows + 4 * scale);
    printf("ALLOCATED MEMORY FOR GAUSS SMOOTH\n");

    /* FILL IN THE VALUES FOR THE GAUSSIAN FILTER */
    int sum = 0;
    printf("------------------------------------------------------\n");
    printf(" FILLING UP GAUSS FILTER VALUES\n");
    for (i = -(gcols / 2); i <= gcols / 2; i++) {
        for (j = -(grows / 2); j <= grows / 2; j++) {
            scale2XY(gauss_filter, i, j) =
                    (1 / (2 * M_PI * sigma * sigma)) * exp(-((i * i) + (j * j)) / (2 * sigma * sigma));
            sum = sum + scale2XY(gauss_filter, i, j);
        }
    }

    printf(" FILLED UP GAUSS FILTER VALUES\n");

    /* INITIALIZE GAUSS SMOOTH*/
    printf("---------------------------------------------------------\n");
    printf("Initializing Gauss smooth matrix\n");
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < ncols; j++) {
            scale2XY(gauss_smooth, j, i) = 0;
        }
    }

    /* CONVOLVE WITH GAUSSIAN FILTER AND STORE IN GAUSS_SMOOTH */
    printf(" ------------------------------------------------------------\n");
    printf(" CONVOLUTION WITH GAUSS FILTER\n");
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < ncols; j++) {
            /* CONVOLUTION*/
            for (c = set; c <= -set; c++) {
                for (r = set; r <= -set; r++) {
                    scale2XY(gauss_smooth, j, i) = scale2XY(gauss_smooth, j, i) +
                                                   (float) scale1XY(grey_scale, j + r, i + c) *
                                                   scale2XY(gauss_filter, r, c);
                }
            }
        }
    }

    /* Write to class member smooth_img with type unsigned char */
    printf(" PURING RESULT\n");
    smooth_img = CImg<unsigned char>(ncols, nrows);
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < ncols; j++) {
            smooth_img(j, i) = (unsigned char) scale2XY(gauss_smooth, j, i);
        }
    }

}


/* REFLECT AN IMAGE ACROSS ITS BORDERS */

/* It is assumed that the image was allocated to be of size at least */
/* (nrows+2*scale) by (ncols+2*scale), and that the image was loaded */
/* into the middle portion of the image, with coordinates, */
/*      0 <= row < nrows, 0 <= col < ncols */
/* thereby leaving empty elements along the borders outside the image */
/* The "reflect" function will then fill in those empty */
/*After reflect is called, the start index of the actual image will be (scale, scale) */
template<class T>
void Canny::reflect(CImg<T> &target, int nrows, int ncols, int c_scale) {
    int i, j;
    for (i = -c_scale; i < 0; i++) {
        for (j = -c_scale; j < 0; j++)
            scale1XY(target, j, i) = scale1XY(target, -j - 1, -i - 1);
        for (j = 0; j < ncols; j++)
            scale1XY(target, j, i) = scale1XY(target, j, -i - 1);
        for (j = ncols; j < ncols + c_scale; j++)
            scale1XY(target, j, i) = scale1XY(target, ncols + ncols - j - 1, -i - 1);
    }
    for (i = 0; i < nrows; i++) {
        for (j = -c_scale; j < 0; j++)
            scale1XY(target, j, i) = scale1XY(target, -j - 1, i);
        for (j = ncols; j < ncols + c_scale; j++)
            scale1XY(target, j, i) = scale1XY(target, ncols + ncols - j - 1, i);
    }
    for (i = nrows; i < nrows + c_scale; i++) {
        for (j = -c_scale; j < 0; j++)
            scale1XY(target, j, i) = scale1XY(target, -j - 1, nrows + nrows - i - 1);
        for (j = 0; j < ncols; j++)
            scale1XY(target, j, i) = scale1XY(target, j, nrows + nrows - i - 1);
        for (j = ncols; j < ncols + c_scale; j++)
            scale1XY(target, j, i) = scale1XY(target, ncols + ncols - j - 1, nrows + nrows - i - 1);
    }
}

CImg<unsigned char> Canny::getGreyScale() {
    return grey_scale;
}

CImg<unsigned char> Canny::getSmoothImage() {

    return smooth_img;
}

/*Returns the gradient and direction matrix*/
pair<CImg<float>, CImg<float>> Canny::sobel() {
    /* ALLOCATING MEM */
    int ncols = origin.width(), nrows = origin.height(), scale = c_scale;
    CImg<float> gradient(ncols + 2, nrows + 2),
            dir(ncols, nrows);


    reflect(gauss_smooth, nrows, ncols, 2 * c_scale);

    for (int i = 0; i < ncols; i++) {
        for (int j = 0; j < nrows; j++) {
            //Sobel
            float x[3][3] = {{-1, 0, 1},
                             {-2, 0, 2},
                             {-1, 0, 1}};
            float y[3][3] = {{-1, -2, -1},
                             {0,  0,  0},
                             {1,  2,  1}};
            float gx = 0, gy = 0;
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    gx += x[r][c] * scale2XY(gauss_smooth, i - 1 + c, j - 1 + r);
                    gy += y[r][c] * scale2XY(gauss_smooth, i - 1 + c, j - 1 + r);
                }
            }
            p1XY(gradient, i, j) = sqrt(gy * gy + gx * gx);
            dir(i, j) = atan(gy / gx) * (180 / M_PI);
        }
    }
    reflect(gradient, nrows, ncols, 1);
    return make_pair(gradient, dir);
}


/* EDGE DETECTION BY CANNY OPERATOR */

CImg<unsigned char> Canny::runCanny() {
    printf(" Edge detection will be performed by Canny method\n");
    printf("-------------------------------------------------------------------\n");

    /* STEP: 1 --- GAUSSIAN SMOOTHING (Already done in constructor) */

    //Sobel
    auto pair = sobel();

    /*Non-Maximum Suppression */
    CImg<unsigned char> edges;
    if (!c_skipNMS) {
        edges = nms(pair.first, pair.second);
    } else {
        edges = CImg<unsigned char>(origin.width(), origin.height());
        cimg_forXY(edges, x, y) {
                edges(x, y) = (unsigned char) p1XY(pair.first, x, y);
            }
    }

    hysThres(edges, 0);
    printf("The lower threshold used = %d \n", min(c_thresh1, c_thresh2));
    printf("The Upper threshold used = %d \n", max(c_thresh1, c_thresh2));
    printf("==========================================\n");
    cimg_forXY(edges, x, y) {
            if (edges(x, y) != 255) edges(x, y) = 0;
        }

    return edges;
}

/*Non-Maximum Suppression */
/* Returns a unsigned char matrix generated from gradient matrix after nms*/
CImg<unsigned char> Canny::nms(CImg<float> &gradient, CImg<float> &dir) {
    int ncols = origin.width(), nrows = origin.height(),
            edgepoints = 0;

    /*  Init all with value 0 */
    CImg<unsigned char> edgeMap(ncols, nrows, 1, 1, 0);

    printf("============================================================\n");
    printf(" Performing Non_maximum-Supression\n");

    cimg_forXY(origin, x, y) {
            float direction = dir(x, y);
            float pt1, pt2;
            if (direction >= -90 && direction <= -67.5) {
                pt1 = p1XY(gradient, x, y - 1);
                pt2 = p1XY(gradient, x, y + 1);
            } else if (direction > -67.5 && direction <= -22.5) {
                pt1 = p1XY(gradient, x + 1, y - 1);
                pt2 = p1XY(gradient, x - 1, y + 1);
            } else if (direction > -22.5 && direction <= 22.5) {
                pt1 = p1XY(gradient, x + 1, y);
                pt2 = p1XY(gradient, x - 1, y);
            } else if (direction > 22.5 && direction <= 67.5) {
                pt1 = p1XY(gradient, x + 1, y + 1);
                pt2 = p1XY(gradient, x - 1, y - 1);
            } else {
                pt1 = p1XY(gradient, x, y - 1);
                pt2 = p1XY(gradient, x, y + 1);
            }
            if (p1XY(gradient, x, y) < abs(pt1) ||
                p1XY(gradient, x, y) < abs(pt2)) {
                edgeMap(x, y) = 0;
            } else {
                edgepoints++;
                edgeMap(x, y) = (unsigned char) p1XY(gradient, x, y);
            }
        }
    printf(" Number of Edgepoints after nms is %d \n", edgepoints);

    return edgeMap;
}

/* Hysteresis Thresholding */
/* Recursively marks edge pixels */
/* Returns the input reference */
void Canny::hysThres(CImg<unsigned char> &edgeMap, int count) {
    bool changed = true;
    unsigned int thresh1 = min(c_thresh2, c_thresh1),
            thresh2 = max(c_thresh2, c_thresh1);
    while (changed) {
        changed = false;
        printf("Run %d\n", count++);

        cimg_forXY(edgeMap, x, y) {
                if (edgeMap(x, y) == 255) continue;
                if (edgeMap(x, y) >= thresh2) {
                    changed = true;
                    edgeMap(x, y) = 255;
                } else if (edgeMap(x, y) <= thresh1) {
                    edgeMap(x, y) = 0;
                } else {
                    // For those in between, connect them if there is a valid pixel in at least
                    // one of their eight neighbors
                    for (int i = x - 1; i <= x + 1; i++) {
                        for (int j = y - 1; j <= y + 1; j++) {
                            if (i < 0 || i == edgeMap.width() || j < 0 || j == edgeMap.height())
                                continue;
                            if (edgeMap(i, j) == 255) {
                                changed = true;
                                edgeMap(x, y) = 255;
                                break;
                            }
                        }
                    }
                }
            }
    }
}

/* Connect all neighboring edges in input */
/* returns the matrix after processing */
CImg<unsigned char> Canny::connectEdges(CImg<unsigned char> &input) {
    int nrows = input.height(), ncols = input.width();
    CImg<unsigned char> res(ncols, nrows, 1, 1, 0);
    int const dirs[8][2] = {{0,  -1},
                            {1,  -1},
                            {1,  0},
                            {1,  1},
                            {0,  1},
                            {-1, 1},
                            {-1, 0},
                            {-1, -1}};
    for (int x = 0; x < ncols; x++) {
        for (int y = 0; y < nrows; y++) {
            if (input(x, y) == 255) {
                res(x, y) = 255;
                continue;
            }

            bool neighbors[3][3] = {{0, 0, 0},
                                    {0, 0, 0},
                                    {0, 0, 0}};
            bool marked[3][3] = {{0, 0, 0},
                                 {0, 0, 0},
                                 {0, 0, 0}};
            int neighbor_count = 0;

            for (int i = 0; i < 8; i++) {
                int new_x = x + dirs[i][0], new_y = y + dirs[i][1];
                if (new_x < 0 || new_x == ncols || new_y < 0 || new_y == ncols)
                    continue;
                if (input(new_x, new_y) == 255) {
                    neighbors[dirs[i][1] + 1][dirs[i][0] + 1] = true;
                }
            }

            //Check whether the current point has two unconnected neighbors
            //If yes, connect this point.
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (neighbors[i][j] == 0) continue;
                    bool flag = true;
                    for (int k = 0; k < 8; k++) {
                        int new_i = i + dirs[k][1], new_j = j + dirs[k][0];
                        if (new_i < 0 || new_i == 3 || new_j < 0 || new_j == 3) continue;
                        if (marked[new_i][new_j]) {
                            flag = false;
                            break;
                        }
                    }
                    if (flag) neighbor_count++;
                    marked[i][j] = true;
                }
            }
            if (neighbor_count >= 2)
                res(x, y) = 255;
        }
    }
    return res;
}

/*Delete all edges shorter than 20 */
/* returns the matrix after processing */
CImg<unsigned char> Canny::deleteShortEdges(CImg<unsigned char> &input) {
    int nrows = input.height(), ncols = input.width();
    const int dirs[][2] = {{0,  -1},
                           {1,  -1},
                           {1,  0},
                           {1,  1},
                           {0,  1},
                           {-1, 1},
                           {-1, 0},
                           {-1, -1}};
    unordered_set<int> found_edges;
    bool found;
    CImg<unsigned char> res(ncols, nrows, 1, 1, 0);

    do {
        found = false;
        int found_x, found_y, dir_index = 2;
        for (int y = 0; y < nrows; y++) {
            for (int x = 0; x < ncols; x++) {
                if (input(x, y) == 255 && found_edges.count(x + y * ncols) == 0) {
                    found_x = x;
                    found_y = y;
                    found = true;
                    found_edges.insert(x + y * ncols);
                    break;
                }
            }
            if (found) break;
        }
        if (!found) break;
        vector<pair<int, int>> current_edges;
        queue<pair<int, int>> unvisited;
        int length = 0;
        unvisited.emplace(found_x, found_y);
        while (!unvisited.empty()) {
            auto curr = unvisited.front();
            unvisited.pop();
            current_edges.push_back(curr);
            ++length;
            for (int i = 0; i < 8; i++, dir_index = (dir_index + 1) % 8) {
                int new_x = curr.first + dirs[dir_index][0];
                int new_y = curr.second + dirs[dir_index][1];
                if (new_x < 0 || new_x == ncols || new_y < 0 || new_y == nrows) continue;
                if (input(new_x, new_y) == 255 && found_edges.count(new_x + new_y * ncols) == 0) {
                    found_edges.insert(new_x + new_y * ncols);
                    unvisited.emplace(new_x, new_y);
                }
            }
        }
        if (length >= 20) {
            for (const auto &point : current_edges) {
                res(point.first, point.second) = 255;
            }
        }
    } while (found);

    return res;
}

CImg<unsigned char> Canny::connectAndDelete(CImg<unsigned char> &input) {
    auto conn = connectEdges(input);
    return deleteShortEdges(conn);
}

