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
    buildHistogramDifferences(imagePaths, gold);
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
 * TODO: Given two images, compute the differences in 8 * 8 * 8 histogram bins.
 */
std::vector<MatND> buildHistogramDifferences(std::vector<std::string> imagePaths, std::vector<sbd::GoldStandardElement> goldStandard) {
    printf("Building histogram differences.\n");

    Histogram histBuilder(8);
    std::cout << "Reading " << imagePaths.size() << " images." << std::endl;
    assert(imagePaths.size() % 2 == 0);

    std::vector<MatND> diffs;
    std::vector<bool> golds;
    for (int i = 0; i < imagePaths.size(); i += 2) {
        Mat image1 = imread(imagePaths[i], CV_LOAD_IMAGE_COLOR);
        Mat image2 = imread(imagePaths[i + 1], CV_LOAD_IMAGE_COLOR);
        bool gold = findGold(imagePaths[i], imagePaths[i + 1], goldStandard);

        MatND hist1 = histBuilder.buildHistogram(image1);
        MatND hist2 = histBuilder.buildHistogram(image2);

        MatND diff = hist1 - hist2;
//        Histogram::displayHistogram(diff);

        diffs.push_back(diff);
        golds.push_back(gold);
    }
    return diffs;
}

/**
 * 3.1
 * TODO: For the two given files find out the gold standard, i.e. whether it is a CUT or not.
 */
bool findGold(std::string path1, std::string path2, std::vector<sbd::GoldStandardElement>) {
    return true;
}

/**
 * 4.
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
 * 5.
 * Evaluate on some held-out test set.
 */
void evaluate() {
    printf("Evaluating on test set .. not yet.\n");
}
