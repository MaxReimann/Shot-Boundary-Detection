
#include <opencv2/opencv.hpp>


class Learner
{
	Learner();

	void train();

	float predict(cv::Mat& sample);

	void showPredictionImage();
};

