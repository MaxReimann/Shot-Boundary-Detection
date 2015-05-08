#include "util.hpp"


using namespace sbd;

//split the feature Vector in train and test set by trainSetRatio (e.g. 0.7 for 70% trainingset and 30% test)
void sbd::splitTrainTestSets(Features &input, float trainSetRatio, Features &trainSet, Features &testSet)
{
	int cols = input.values.cols;
	int rows = input.values.rows;
	assert(cols > 0);
	cv::Mat trainValues = cv::Mat(0, cols, CV_32FC1);
	cv::Mat testValues = cv::Mat(0, cols, CV_32FC1);
	cv::Mat trainLabels = cv::Mat(0,1, CV_32FC1);
	cv::Mat testLabels = cv::Mat(0, 1, CV_32FC1);

	for (int i = 0; i < rows; i++)
	{
		float currentRatio = i / static_cast<float>(rows);
		if (currentRatio < trainSetRatio)
		{
			trainValues.push_back(input.values.row(i));
			trainLabels.push_back(input.classes.row(i));
		}
		else
		{
			testValues.push_back(input.values.row(i));
			testLabels.push_back(input.classes.row(i));
		}
	}

	trainSet.values = trainValues;
	trainSet.classes = trainLabels;
	testSet.values = testValues;
	testSet.classes = testLabels;

}
