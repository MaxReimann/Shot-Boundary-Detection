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

    int width = 512;
    int height = 256;

    cv::Mat histImage(height, width, CV_8UC1, cv::Scalar(0));
    cv::Mat outHist;
    //cv::normalize(hist, hist, height, 0.0, cv::NORM_L1, -1, cv::Mat());

    float maxEl = 0.0;
    for (auto it = hist.begin<float>(); it != hist.end<float>(); it++)
        if (maxEl < *it)
            maxEl = *it;

    float scale = height / maxEl;

    auto it = hist.begin<float>();
    float last = *it++;

    for (int i = 1; it != hist.end<float>(); it++, i++) {
        cv::line(histImage, cv::Point(i - 1, height - cvRound(last * scale)),
            cv::Point(i, height - cvRound(*it * scale)),
            cv::Scalar(255),2, 8, 0);
        float last = *it;
        printf("%f,", *it * scale);
    }

    cv::namedWindow("Result", 1);
    cv::imshow("Result", histImage);
    cv::waitKey(0);
}

void Histogram::plotRGBHistogram(const cv::Mat &srcImage, int width, int height) {
    //! the number of rows and columns or (-1, -1) when the Mat has more than 2 dimensions
    //int histSize = hist.rows;

    //as it is not possible to visualize a 3d volumetric rendering of the 3d histogram, plot 3 1d histograms


    /// Separate the image in 3 places ( B, G and R )
    std::vector<cv::Mat> bgrPlanes;
    cv::split(srcImage, bgrPlanes);

    /// Establish the number of bins
    int histSize = 256;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 256 };
    const float* histRange = { range };

    bool uniform = true; bool accumulate = false;

    cv::Mat bHist, gHist, rHist;

    /// Compute the histograms:
    cv::calcHist(&bgrPlanes[0], 1, 0, cv::Mat(), bHist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgrPlanes[1], 1, 0, cv::Mat(), gHist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgrPlanes[2], 1, 0, cv::Mat(), rHist, 1, &histSize, &histRange, uniform, accumulate);

    // Draw the histograms for B, G and R
    int binW = cvRound((double)width / histSize);

    cv::Mat histImage(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

    /// Normalize the result to [ 0, histImage.rows ]
    cv::normalize(bHist, bHist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(gHist, gHist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(rHist, rHist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    /// Draw for each channel
    for (int i = 1; i < histSize; i++)
    {
        line(histImage, cv::Point(binW*(i - 1), height - cvRound(bHist.at<float>(i - 1))),
            cv::Point(binW*(i), height - cvRound(bHist.at<float>(i))),
            cv::Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, cv::Point(binW*(i - 1), height - cvRound(gHist.at<float>(i - 1))),
            cv::Point(binW*(i), height - cvRound(gHist.at<float>(i))),
            cv::Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, cv::Point(binW*(i - 1), height - cvRound(rHist.at<float>(i - 1))),
            cv::Point(binW*(i), height - cvRound(rHist.at<float>(i))),
            cv::Scalar(0, 0, 255), 2, 8, 0);
    }

    cv::namedWindow("Result", 1 );
    cv::imshow("Result", histImage);
    cv::waitKey(0);
}

cv::Mat Histogram::buildHistogram(const cv::Mat& image) {
    int nrImages = 1;
    const int channels[] = { 0, 1, 2 };
    cv::Mat mask;
    cv::Mat hist;
    int histDimensionality = 3;
    const int histSizes[] = { m_histSize, m_histSize, m_histSize };
    float range[] = { 0, 256 };
    const float* ranges[] = { range, range, range };
    cv::calcHist(&image, nrImages, channels, mask,
             hist, histDimensionality, histSizes, ranges, true, false);

    return hist;
}

cv::Mat Histogram::buildHistogram1Channel(const cv::Mat& image) {
    int nrImages = 1;
    const int channels[] = { 0 };
    cv::Mat mask;
    cv::Mat hist;
    int histDimensionality = 1;
    const int histSizes[] = { m_histSize };
    float range[] = { 0, 256 };
    const float* ranges[] = { range };
    cv::calcHist(&image, nrImages, channels, mask,
        hist, histDimensionality, histSizes, ranges, true, false);

    return hist;
}

cv::Mat Histogram::convertMat(const cv::Mat& hist) {
    // resize mat from a 3d Mat to a nx1 Mat. Only changes headers
    cv::Mat oneDimMat(1, m_histSize * m_histSize * m_histSize, CV_32FC1,
        hist.data);  // void cast, else false data type will be assumed
    assert(oneDimMat.isContinuous());

    return oneDimMat;
}

cv::Mat Histogram::convertMat1Channel(const cv::Mat& hist) {
    // resize mat from a 3d Mat to a nx1 Mat. Only changes headers
    cv::Mat oneDimMat(1, m_histSize, CV_32FC1,
        hist.data);  // void cast, else false data type will be assumed
    assert(oneDimMat.isContinuous());

    return oneDimMat;
}

std::vector<float> getAbsChanges(const cv::Mat& diffs) {

}
