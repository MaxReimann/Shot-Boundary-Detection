#include "histogram.hpp"

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <cmath>
#include <src/util.hpp>

using namespace sbd;


Histogram::Histogram(int histSize) {
    this->m_histSize = histSize;
}
int Histogram::getHistSize() {
    return this->m_histSize;
}

cv::Mat Histogram::getDiff(std::string image1Path, std::string image2Path) {
    cv::Mat image1 = cv::imread(image1Path, CV_LOAD_IMAGE_COLOR);
    cv::Mat image2 = cv::imread(image2Path, CV_LOAD_IMAGE_COLOR);
    assert(image1.total() > 0);
    assert(image2.total() > 0);

    // extract the luma component
    cv::Mat YUVimage1;
    cv::Mat YUVimage2;
    cv::cvtColor(image1, YUVimage1, CV_BGR2YCrCb);
    cv::cvtColor(image2, YUVimage2, CV_BGR2YCrCb);

    std::vector<cv::Mat> channels1;
    std::vector<cv::Mat> channels2;
    cv::split(YUVimage1, channels1);
    cv::split(YUVimage2, channels2);

    cv::Mat y1 = channels1[0];
    cv::Mat y2 = channels2[0];

    // build histograms
    cv::Mat hist1 = buildHistogram1Channel(y1);
    cv::Mat oneDimHist1 = convertMat1Channel(hist1);
    cv::Mat hist2 = buildHistogram1Channel(y2);
    cv::Mat oneDimHist2 = convertMat1Channel(hist2);

//    // build histogram (color)
//    cv::Mat hist1 = histBuilder.buildHistogram(image1);
//    cv::Mat oneDimHist1 = histBuilder.convertMat(hist1);
//    cv::Mat hist2 = histBuilder.buildHistogram(image1);
//    cv::Mat oneDimHist2 = histBuilder.convertMat(hist2);

    cv::Mat diff = oneDimHist1 - oneDimHist2;

    return diff;
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
    cv::Mat oneDimMat(1, m_histSize, CV_32FC1, hist.data);
    assert(oneDimMat.isContinuous());

    return oneDimMat;
}

std::vector<float> Histogram::getAbsChanges(const cv::Mat& diffs) {
    std::vector<float> absValues;

    for (int i = 0; i < diffs.rows; ++i) {
        float sum = 0;
        for (int j = 0; j < diffs.cols; ++j) {
            float val = diffs.at<float>(i, j);
            sum += std::abs(val);
        }

        absValues.push_back(sum);
    }

    return absValues;
}

void Histogram::drawAbsChanges(std::vector<float> absChanges, const cv::Mat& golds, std::vector<std::string> frameNumbers) {
    assert(absChanges.size() == golds.size().height);

    int MARGIN = 10;
    int BINSIZE = 1;
    int imageWidth = BINSIZE * absChanges.size() + 2 * MARGIN;
    int LEGEND_HEIGHT = 50;
    int imageHeight = 400;

    cv::Mat histImage(imageHeight + LEGEND_HEIGHT, imageWidth, CV_8UC3, cv::Scalar(0, 0, 0));

    cv::normalize(absChanges, absChanges, 0, imageHeight, cv::NORM_MINMAX, -1, cv::Mat());

    for (unsigned int i = 0; i < absChanges.size(); i++) {
        int absChange = cvRound(absChanges.at(i));
        float topPercent = static_cast<float>(absChange) / imageHeight;
        bool isTop = topPercent > 0.75;
        cv::Point start(MARGIN + BINSIZE * i, imageHeight - absChange);
        cv::Point end(MARGIN + BINSIZE * i, imageHeight);

        cv::Scalar lineColor = golds.at<float>(i) ? cv::Scalar(0, 165, 255) : cv::Scalar(255, 255, 255);

        cv::line(histImage, start, end, lineColor, BINSIZE, 8);

        if (i % 100 == 0) {
            cv::putText(histImage, frameNumbers[i], cv::Point(MARGIN + BINSIZE * i, imageHeight + LEGEND_HEIGHT / 2), CV_FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(255, 255, 255));
        }
        if (isTop) {
            cv::putText(histImage, frameNumbers[i], cv::Point(MARGIN + BINSIZE * i, 10), CV_FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(255, 255, 255));
        }

    }

    cv::imwrite("../resources/abs-changes.png", histImage);

#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif

}
