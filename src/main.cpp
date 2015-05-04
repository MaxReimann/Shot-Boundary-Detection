#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "histogram/histogram.h"

using namespace cv;


// dummy stuff, to see if OpenCV is working

int main(int argc, char** argv) {
    Histogram hist;
    hist.buildHistogram();
}


int openCvSample(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
    image = imread(argv[1], 1);

    if (!image.data) {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}
