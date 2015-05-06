#include "histogram.hpp"

#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace sbd;


Histogram::Histogram(int histSize) {
    this->m_histSize = histSize;
}
int Histogram::getHistSize() {
    return this->m_histSize;
}

void Histogram::displayHistogram(const cv::Mat &hist) {
    std::cout << "Histogram size = " << hist.total() << ": [" << hist.size[0] << " x " << hist.size[1] << " x " << hist.size[2] << "]" << std::endl;
//    for (MatConstIterator_<float> it = hist.begin<float>(); it != hist.end<float>(); it++) {
//        cout << *it << "\n";
//    }
//    cout << std::flush;
}

void Histogram::plotHistogram(const cv::Mat &hist, int width, int height) {
    return;
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

cv::Mat Histogram::buildHistogram(const cv::Mat& image) {
    int nrImages = 1;
    int channels[] = { 0, 1, 2 };
    cv::Mat mask;
    cv::Mat hist;
    int histDimensionality = 3;
    int histSizes[] = { this->m_histSize, this->m_histSize, this->m_histSize };
    float range[] = { 0, 256 };
    const float* ranges[] = { range, range, range };
    calcHist(&image, nrImages, channels, mask,
             hist, histDimensionality, histSizes, ranges, true, false);

    return hist;
}
