#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include "main.hpp"
#include "histogram/histogram.hpp"
#include "gold_standard/file_reader.hpp"
#include "gold_standard/gold_standard_element.hpp"
#include "svm/svm.hpp"

using namespace sbd;


int main(int argc, char** argv) {
    std::vector<sbd::GoldStandardElement> gold = readGoldStandard();
    std::vector<std::string> imagePaths = getFileNames();
    Features features = buildHistogramDifferences(imagePaths, gold);
    trainSVM(features);
    evaluate();

	// wait for key, so we can read the console output
	system("pause");
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
    std::string dir = "../resources/frames/anni009/";

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
Features buildHistogramDifferences(std::vector<std::string> &imagePaths, std::vector<sbd::GoldStandardElement> &goldStandard) {
    printf("Building histogram differences.\n");

    Histogram histBuilder(8);
    std::cout << "Reading " << imagePaths.size() << " images." << std::endl;
    assert(imagePaths.size() % 2 == 0);

    cv::Mat diffs;
	cv::Mat golds;
    for (int i = 0; i < imagePaths.size(); i += 2) {
		cv::Mat image1 = cv::imread(imagePaths[i], CV_LOAD_IMAGE_COLOR);
		cv::Mat image2 = cv::imread(imagePaths[i + 1], CV_LOAD_IMAGE_COLOR);
		
        float gold = findGold(imagePaths[i], imagePaths[i + 1], goldStandard);

		cv::Mat hist1 = histBuilder.buildHistogram(image1);
		cv::Mat hist2 = histBuilder.buildHistogram(image2);


		cv::Mat diff = hist1 - hist2;
		//Histogram::displayHistogram(hist1);

		for (auto it = diff.begin<float>(); it != diff.end<float>(); it++)
			if (*it != 0.0)
				printf("%f,", *it);

        diffs.push_back(diff);
        golds.push_back(gold);
    }
    Features features = {golds, diffs};
    return features;
}

/**
 * 3.1
 * TODO: For the two given files find out the gold standard, i.e. whether it is a CUT or not.
 */
bool findGold(std::string path1, std::string path2, std::vector<sbd::GoldStandardElement> &gold) {
	std::string frameNr1 = boost::filesystem::path(path1).stem().string();
	std::string frameNr2 = boost::filesystem::path(path2).stem().string();

	for (int i = 0; i < gold.size(); i++) {
		if (frameNr1 == std::to_string(gold[i].startFrame) && frameNr2 == std::to_string(gold[i].endFrame)) 
			return true;
	}
	return false;
}

/**
 * 4.
 * TODO: Trains the SVM with the histogram differences and the gold standard.
 */

void trainSVM(Features &features) {
    printf("Training SVM.\n");
	// Set up training data

	cv::Mat trainMat = features.values;
	cv::Mat labelsMat = features.classes;

	//for (auto it = trainMat.begin<float>(); it != trainMat.end<float>(); it++)
	//	if (*it != 0)
	//		printf("%f,", *it);

	assert(trainMat.isContinuous());

	SVMLearner svm;
	svm.train(trainMat, labelsMat);
	//svm.plotDecisionRegions();
}

/**
 * 5.
 * Evaluate on some held-out test set.
 */
void evaluate() {
    printf("Evaluating on test set .. not yet.\n");
}
