#include "../forwarddeclarations.h"

namespace sbd
{
	class Histogram {

	public:
		cv::MatND buildHistogram(const cv::Mat& image);
	};

}