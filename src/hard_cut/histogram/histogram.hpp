#pragma once

#include "../../forwarddeclarations.hpp"

namespace sbd {
    class Histogram {
    private:
        int m_histSize;
        bool m_blackAndWhite;
    public:
        Histogram(int, bool);
        int getHistSize();
        cv::Mat buildHistogram(const cv::Mat& image);
        cv::Mat buildHistogram1Channel(const cv::Mat& image);
        cv::Mat convertMat(const cv::Mat& hist);
        cv::Mat convertMat1Channel(const cv::Mat& hist);
        static std::vector<float> getAbsChanges(const cv::Mat& diffs);
        void drawAbsChanges(std::vector<float> absChanges, const cv::Mat& golds, std::vector<std::string> frameNumbers);
        cv::Mat getDiff(std::string image1, std::string image2);
    };
}
