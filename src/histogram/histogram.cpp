
#include <stdio.h>
#include "histogram.h"

using namespace cv;

MatND Histogram::buildHistogram(const Mat image) {
    printf("Hello World\n");

    int channels[] = { 0 };
    MatND hist;
    int bins = 30;
    int histSize[] = { bins };

    float range[] = { 0, 256 };
    const float* ranges[] = { range };

    calcHist(&image, 1, channels, Mat(),
             hist, 1, histSize, ranges, true, false);

    return hist;
}
