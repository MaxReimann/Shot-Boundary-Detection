#include "histogram.h"

#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace sbd;


Histogram::Histogram(int histSize) {
    this->m_histSize = histSize;
}
int Histogram::getHistSize() {
    return this->m_histSize;
}

cv::MatND Histogram::buildHistogram(const cv::Mat& image) {
    int channels[] = { 0 };
    cv::MatND hist;

    float range[] = { 0, 256 };
    const float* ranges[] = { range };

    calcHist(&image, 1, 0, cv::Mat(),
             hist, 1, &this->m_histSize, ranges, true, false);

    return hist;
}
