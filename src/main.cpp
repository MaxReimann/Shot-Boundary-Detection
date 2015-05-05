#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "histogram/histogram.h"
#include "svm/svm.h"

using namespace sbd;
using namespace cv;
using namespace std;

void plotHistogram(Mat);


void classifyExample() {
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
    Mat image;
    image = imread("../resources/cat.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    imshow("Image", image);
    waitKey(0);

    Histogram histBuilder(8);
    MatND hist = histBuilder.buildHistogram(image);
    cout << "hist = " << endl << " "  << hist << endl;

    printf("Rows: %d, Columns: %d\n", hist.rows, hist.cols);

    plotHistogram(hist);
    classifyExample();
    return 0;
}

void plotHistogram(Mat hist) {
    int hist_w = 1000;
    int hist_h = 400;
    int histSize = hist.rows;
    int bin_w = cvRound((double) hist_w / histSize);

    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0));
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    printf("After normalization:\n");
    cout << "hist = " << endl << " "  << hist << endl;

    for  (int i = 1; i <= histSize; i++) {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))) ,
                        Point(bin_w * (i),     hist_h - cvRound(hist.at<float>(i))),
                        Scalar(255), 2, 8, 0);
    }

    namedWindow("Result", 1 );
    imshow("Result", histImage);
    waitKey(0);
}
