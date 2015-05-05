#include "../forwarddeclarations.h"

namespace sbd {
	class Histogram {
    private:
        int m_histSize;
	public:
        Histogram(int);
        int getHistSize();
		cv::MatND buildHistogram(const cv::Mat& image);
	};
}
