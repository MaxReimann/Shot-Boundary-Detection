#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "histogram/histogram.h"

using namespace cv;

int main(int argc, char** argv) {
    Mat image;
    image = imread("../resources/cat.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat b = image;

//    imshow("Image", image);
//    waitKey(0);

    Histogram histBuilder;
    MatND hist = histBuilder.buildHistogram(image);
    return 0;
}
