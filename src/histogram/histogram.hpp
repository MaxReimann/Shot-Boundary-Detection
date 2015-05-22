#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd {
    class Histogram {
    private:
        int m_histSize;
    public:
        Histogram(int);
        int getHistSize();
        cv::Mat buildHistogram(const cv::Mat& image);
        cv::Mat buildHistogram1Channel(const cv::Mat& image);
        static void displayHistogram(const cv::Mat& hist);
        static void plotRGBHistogram(const cv::Mat &srcImage, int width, int height);
        cv::Mat convertMat(const cv::Mat& hist);
        cv::Mat convertMat1Channel(const cv::Mat& hist);
    };
}
