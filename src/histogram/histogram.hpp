#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd {
	class Histogram {
    private:
        int m_histSize;
	public:
        Histogram(int);
        int getHistSize();
		cv::MatND buildHistogram(const cv::Mat& image);
        static void plotHistogram(const cv::Mat& hist, int width, int height);
	};
}
