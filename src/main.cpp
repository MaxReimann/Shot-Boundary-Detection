#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include "main.hpp"
#include "histogram/histogram.hpp"
#include "gold_standard/file_reader.hpp"
#include "gold_standard/gold_standard_element.hpp"
#include "svm/svm.hpp"

using namespace cv;
using namespace sbd;
using namespace std;

int main(int argc, char** argv) {
    std::vector<sbd::GoldStandardElement> gold = readGoldStandard();
    std::vector<std::string> imagePaths = getFileNames();
    Mat image = readImages(imagePaths);
    buildHistogramDifferences(image);
    trainSVM();
    evaluate();

    return 0;
}

/**
 * 1.
 * Reads the gold standard and returns it in some easy format.
 */
std::vector<sbd::GoldStandardElement> readGoldStandard() {
    printf("Reading gold standard.\n");

    FileReader fileReader;
    std::vector<sbd::GoldStandardElement> goldStandard = fileReader.readDir("../resources/truth/");

//    for (std::vector<GoldStandardElement>::size_type i = 0; i != goldStandard.size(); i++) {
//        std::cout << "Name: "   << goldStandard[i].name
//                  << " Type: "  << goldStandard[i].type
//                  << " Start: " << goldStandard[i].startFrame
//                  << " End: "   << goldStandard[i].endFrame
//                  << "\n";
//    }
    return goldStandard;
}

/**
 * 2.
 * Read the frame file names recursively.
 */
std::vector<std::string> getFileNames() {
    printf("Getting frame file names.\n");

    std::vector<std::string> imagePaths;
    std::string extension = ".jpg";
    std::string dir = "../resources/frames/";

    boost::filesystem::recursive_directory_iterator rdi(dir);
    boost::filesystem::recursive_directory_iterator end_rdi;
    for (; rdi != end_rdi; rdi++) {
        if (extension.compare((*rdi).path().extension().string()) == 0) {
            imagePaths.push_back((*rdi).path().string());
        }
    }

    return imagePaths;
}

/**
 * 3.
 * TODO: Given a list of file names, read all the corresponding images as matrices.
 * Maybe use iterator, so we do not read all at once, but only on demand?
 */
Mat readImages(std::vector<std::string> imagePaths) {
    printf("Reading images.\n");
    Mat image;
    image = imread("../resources/cat.jpg", CV_LOAD_IMAGE_COLOR);

//    imshow("Image", image);
//    waitKey(0);
    return image;
}

/**
 * 4.
 * TODO: Given two images, compute the differences in 8 * 8 * 8 histogram bins.
 */
void buildHistogramDifferences(Mat image) {
    printf("Building histogram differences.\n");

    Histogram histBuilder(8);
    MatND hist = histBuilder.buildHistogram(image);

//    for (MatConstIterator_<float> it = hist.begin<float>(); it != hist.end<float>(); it++) {
//        cout << *it << "\n";
//    }
//    cout << std::flush;

    Histogram::displayHistogram(hist);
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
//	svm.plotDecisionRegions();
}

/**
 * 6.
 * Evaluate on some held-out test set.
 */
void evaluate() {
    printf("Evaluating on test set .. not yet.\n");
}
