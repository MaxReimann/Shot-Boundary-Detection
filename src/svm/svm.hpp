#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd
{

	class SVMLearner
	{
	public:
		SVMLearner();

		void train(cv::Mat& trainingData, cv::Mat& labels);
		float predict(cv::Mat& sample);
		void plotDecisionRegions();

	private:
		CvSVMParams m_params;
		CvSVM m_svm;
	};

}
