#include "svm.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

using namespace sbd;


SVMLearner::SVMLearner()
{
    // Set up SVM's parameters
    m_params.svm_type = CvSVM::C_SVC;
    m_params.kernel_type = CvSVM::LINEAR;
    m_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);


	// create a 3x3 double-precision identity matrix
}

void SVMLearner::train(cv::Mat& trainingData, cv::Mat& labels)
{
    m_svm.train_auto(trainingData, labels, cv::Mat(), cv::Mat(), m_params);
}

float SVMLearner::predict(cv::Mat& sample)
{
    return m_svm.predict(sample);
}

void SVMLearner::plotDecisionRegions(cv::Mat &testData, cv::Mat &testLabels)
{
    // Data for visual representation
    int width = 512, height = 512;
    cv::Mat image = cv::Mat::zeros(height, width, CV_8UC3);

    cv::Scalar black(0, 0, 0), green(0, 255, 0);
	//Show the decision regions given by the SVM

    // Show the training data
    int thickness = -1;
    int lineType = 8;
	for (int i = 0; i < image.rows; ++i)
		{
		for (int j = 0; j < image.cols; ++j)
		{
			
			image.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
			//if (response == 1)
			//	image.at<cv::Vec3b>(i, j) = green;
			//else if (response == -1)
			//	image.at<cv::Vec3b>(i, j) = blue;
		}
	}
	float maxV = 0;
	for (int rowIndex = 0; rowIndex < testData.rows; rowIndex++) {
		cv::Mat mTest = testData.row(rowIndex);
		if (mTest.at<float>(0, 0) > maxV)
			maxV = mTest.at<float>(0, 0);
	}
		


	for (int rowIndex = 0; rowIndex < testData.rows; rowIndex++) {
		cv::Mat mTest = testData.row(rowIndex);
		float predicted = m_svm.predict(mTest);
		//float actual = labelsMat.at<float>(rowIndex, 0);
		float val = mTest.at<float>(0, 0) / maxV * height;
		if (predicted == 0)
			circle(image, cv::Point(rowIndex % width,val) , 1, black, thickness, lineType);
		else
		{
			circle(image, cv::Point(rowIndex % width, val), 5, green, thickness, lineType);
			//std::cout << val << std::endl;
		}

		//        printf("Predicted %f   Actual: %f\n", predicted, actual);
	}


    //circle(image, cv::Point(255, 10), 5, cv::Scalar(255, 255, 255), thickness, lineType);
    //circle(image, cv::Point(501, 255), 5, cv::Scalar(255, 255, 255), thickness, lineType);
    //circle(image, cv::Point(10, 501), 5, cv::Scalar(255, 255, 255), thickness, lineType);

    // Show support vectors
    thickness = 2;
    lineType = 8;
    int c = m_svm.get_support_vector_count();

    for (int i = 0; i < c; ++i)
    {
        const float* v = m_svm.get_support_vector(i);
        circle(image, cv::Point(256,v[0]), 6, cv::Scalar(128, 128, 128), thickness, lineType);
    }

    cv::imshow("SVM Simple Example", image); // show it to the user
    cv::waitKey(0);
}
