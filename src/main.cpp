#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "histogram/histogram.h"
#include "svm/svm.h"

using namespace sbd;


void classifyExample()
{
	// Set up training data
	float labels[4] = { 1.0, -1.0, -1.0, -1.0 };
	cv::Mat labelsMat(4, 1, CV_32FC1, labels);

	float trainingData[4][2] = { { 501, 10 }, { 255, 10 }, { 501, 255 }, { 10, 501 } };
	cv::Mat trainingDataMat(4, 2, CV_32FC1, trainingData);

	SVMLearner svm;
	svm.train(trainingDataMat, labelsMat);
	svm.plotDecisionRegions();
}


int main(int argc, char** argv) {
    cv::Mat image;
    image = cv::imread("../resources/cat.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat b = image;

//    imshow("Image", image);
//    waitKey(0);

    Histogram histBuilder;
    cv::MatND hist = histBuilder.buildHistogram(image);

	classifyExample();
    
	return 0;
}


