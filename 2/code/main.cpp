#include <thread>
#include "canny.h"


void error(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

Canny process_args(int argc, char **argv) {
    bool skipNMS = false, histh = false;
    char *str;
    int thresh = INT_MAX, thresh2, scale;
    float sigma;
    char err[100];
    snprintf(err, 100, "Usage:%s [-skipNMS] -t thresh_low thresh_high filename", argv[0]);
    while (--argc > 0 && **(argv + 1) == '-') {
        switch ((*++argv)[1]) {

            case 's':
                skipNMS = true;
                printf("skipNMS\n");
                break;

            case 't':
                printf("Setting threshold \n");
                printf(" %s is the threshold\n", *++argv);
                argc--;
                str = *(argv);
                thresh = atoi(str);
                str = *(++argv);
                argc--;
                printf(" The str is %s\n", str);

                if (str[0] <= '9' && str[0] >= '0') {
                    histh = true;
                    printf("Performing Hysteresis Thresholding\n");
                    thresh2 = atoi(str);
                } else {
                    histh = false;
                    printf("Using only one global threshold\n");
                    thresh2 = thresh;
                    printf(" Mention the upper threshold also\n");

                    error(err);

                    exit(0);
                }

                break;

            case 'c':
                printf(" Proceeding with Canny Edge-detection algorithm\n");
                break;

            default:
                error(err);
                break;
        }
    }

    if (thresh == INT_MAX) {
        cerr << err << endl;
        error("Thresh not set.");
    }

    /* CANNY ASK USER FOR SIGMA VALUE */
    printf(" Choose a sigma value ( Caorseness parameter) :");
    scanf("%f", &sigma);
    printf(" Choose a scale for the sigma: ");
    scanf("%d", &scale);


    printf("------------------------------------------------\n");
    printf("Opening image file\n");
    string filename;
    printf(" Making decesion\n");
    if (histh) {
        if (argc == 0) {
            cerr << "File not set!" << endl;
            error(err);
        }
        filename = *(++argv);
    } else {
        filename = str;
    }

    if (filename == "") {
        cerr << "File not set!" << endl;
        error(err);
    }

    return Canny(filename, skipNMS, thresh, thresh2, scale, sigma);

}


int main(int argc, char **argv) {

    Canny c = process_args(argc, argv);
    c.getSmoothImage().save("smoothed_image.bmp");
    c.sobel().first.save("sobel.bmp");
    auto edges = c.runCanny();
    edges.save("ecanny_map.bmp");
    thread disp_can([&]() {
        edges.display("Canny edges");
    });

    auto addition = Canny::connectAndDelete(edges);

    thread disp_add([&]() {
        addition.display("Addition");
    });

    addition.save("linesDeleted.bmp");

    disp_can.join();
    disp_add.join();
}