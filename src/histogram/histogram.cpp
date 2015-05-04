#include "histogram.h"

#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace sbd;

cv::MatND Histogram::buildHistogram(const cv::Mat& image) {
    printf("Hello World\n");

    int channels[] = { 0 };
    cv::MatND hist;
    int bins = 30;
    int histSize[] = { bins };

    float range[] = { 0, 256 };
    const float* ranges[] = { range };

    calcHist(&image, 1, channels, cv::Mat(),
             hist, 1, histSize, ranges, true, false);

    return hist;
}
