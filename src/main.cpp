#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "main.hpp"
#include "histogram/histogram.h"
#include "svm/svm.h"

using namespace sbd;
using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    getFileNames();
    Mat image = readImages();
    buildHistogramDifferences(image);
    readGoldStandard();
    trainSVM();
    evaluate();

    return 0;
}

/**
 * 1.
 * Read the frame file names recursively.
 */
void getFileNames() {
    printf("Getting frame file names .. not yet.\n");
}

/**
 * 2.
 * TODO: Given a list of file names, read all the corresponding images as matrices.
 * Maybe use iterator, so we do not read all at once, but only on demand?
 */
Mat readImages() {
    printf("Reading images.\n");
    Mat image;
    image = imread("../resources/cat.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    imshow("Image", image);
    waitKey(0);
    return image;
}

/**
 * 3.
 * TODO: Given two images, compute the differences in 8 * 8 * 8 histogram bins.
 */
void buildHistogramDifferences(Mat image) {
    printf("Building histogram differences.\n");

    Histogram histBuilder(8);
    MatND hist = histBuilder.buildHistogram(image);
//    cout << "hist = " << endl << " "  << hist << endl;
//    printf("Rows: %d, Columns: %d\n", hist.rows, hist.cols);

    Histogram::plotHistogram(hist, 1000, 400);
}

/**
 * 4.
 * TODO: Reads the gold standard and returns it in some easy format.
 */
void readGoldStandard() {
    printf("Reading gold standard .. not yet.\n");
}

/**
 * 5.
 * TODO: Trains the SVM with the histogram differences and the gold standard.
 */

void trainSVM() {
    printf("Training SVM.\n");
	// Set up training data
	float labels[4] = { 1.0, -1.0, -1.0, -1.0 };
	cv::Mat labelsMat(4, 1, CV_32FC1, labels);

	float trainingData[4][2] = { { 501, 10 }, { 255, 10 }, { 501, 255 }, { 10, 501 } };
	cv::Mat trainingDataMat(4, 2, CV_32FC1, trainingData);

	SVMLearner svm;
	svm.train(trainingDataMat, labelsMat);
	svm.plotDecisionRegions();
}

/**
 * 6.
 * Evaluate on some held-out test set.
 */
void evaluate() {
    printf("Evaluating on test set .. not yet.\n");
}

