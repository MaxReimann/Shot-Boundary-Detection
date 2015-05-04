#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "histogram/histogram.h"

using namespace cv;


// dummy stuff, to see if OpenCV is working

int main(int argc, char** argv) {
    Histogram hist;
    hist.buildHistogram();

    Mat image;
    image = imread("../resources/cat.jpg", 1);

    if (!image.data) {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}
