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

void Histogram::plotHistogram(const cv::Mat &hist, int width, int height) {
    int histSize = hist.rows;
    int binWidth = cvRound((double) width / histSize);

    cv::Mat histImage(height, width, CV_8UC1, cv::Scalar(0));
    normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    for (int i = 1; i <= histSize; i++) {
        cv::line(histImage, cv::Point(binWidth * (i - 1), height - cvRound(hist.at<float>(i - 1))) ,
                            cv::Point(binWidth * (i),     height - cvRound(hist.at<float>(i))),
                            cv::Scalar(255), 2, 8, 0);
    }

    cv::namedWindow("Result", 1 );
    cv::imshow("Result", histImage);
    cv::waitKey(0);
}

cv::MatND Histogram::buildHistogram(const cv::Mat& image) {
    cv::MatND hist;

    float range[] = { 0, 256 };
    const float* ranges[] = { range };

    calcHist(&image, 1, 0, cv::Mat(),
             hist, 1, &this->m_histSize, ranges, true, false);

    return hist;
}
