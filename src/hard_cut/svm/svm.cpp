#include "svm.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

using namespace sbd;


SVMLearner::SVMLearner()
{
    // Set up SVM's parameters
    m_params.svm_type = CvSVM::C_SVC;
    m_params.kernel_type = CvSVM::RBF;
    m_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

	cv::Mat weights = cv::Mat(1, 2, CV_32FC1);
	weights.at<float>(cv::Point(0, 0)) = 10.f;
	weights.at<float>(cv::Point(1, 0)) = 1.f;
	CvMat *classWeights = new CvMat(weights);
	m_params.class_weights = classWeights;

	std::cout << "class weights =" << " " << weights << std::endl << std::endl;


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

void SVMLearner::plotDecisionRegions()
{
    // Data for visual representation
    int width = 512, height = 512;
    cv::Mat image = cv::Mat::zeros(height, width, CV_8UC3);

    cv::Vec3b green(0, 255, 0), blue(255, 0, 0);
    // Show the decision regions given by the SVM
    //for (int i = 0; i < image.rows; ++i)
    //{
    //  for (int j = 0; j < image.cols; ++j)
    //  {
    //      cv::Mat sampleMat = (cv::Mat_<float>(1, 2) << j, i);
    //      float response = m_svm.predict(sampleMat);

    //      if (response == 1)
    //          image.at<cv::Vec3b>(i, j) = green;
    //      else if (response == -1)
    //          image.at<cv::Vec3b>(i, j) = blue;
    //  }
    //}


    // Show the training data
    int thickness = -1;
    int lineType = 8;
    circle(image, cv::Point(501, 10), 5, cv::Scalar(0, 0, 0), thickness, lineType);
    circle(image, cv::Point(255, 10), 5, cv::Scalar(255, 255, 255), thickness, lineType);
    circle(image, cv::Point(501, 255), 5, cv::Scalar(255, 255, 255), thickness, lineType);
    circle(image, cv::Point(10, 501), 5, cv::Scalar(255, 255, 255), thickness, lineType);

    // Show support vectors
    thickness = 2;
    lineType = 8;
    int c = m_svm.get_support_vector_count();

    for (int i = 0; i < c; ++i)
    {
        const float* v = m_svm.get_support_vector(i);
        circle(image, cv::Point((int)v[0], (int)v[1]), 6, cv::Scalar(128, 128, 128), thickness, lineType);
    }

    cv::imshow("SVM Simple Example", image); // show it to the user
    cv::waitKey(0);
}
