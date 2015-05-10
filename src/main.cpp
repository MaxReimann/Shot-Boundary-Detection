#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "main.hpp"
#include "histogram/histogram.hpp"
#include "gold_standard/file_reader.hpp"
#include "gold_standard/gold_standard_element.hpp"
#include "svm/svm.hpp"
#include "util.hpp"

using namespace sbd;


int main(int argc, char** argv) {
    std::vector<sbd::GoldStandardElement> gold = readGoldStandard();
    std::vector<std::string> imagePaths = getFileNames();
    Features features = buildHistogramDifferences(imagePaths, gold);

    Features trainSet, testSet;
    splitTrainTestSets(features, 0.7, trainSet, testSet);
    SVMLearner* learner = trainSVM(trainSet);
    evaluate(testSet, learner);

    // wait for key, so we can read the console output
    cv::waitKey(0);
//    system("pause"); // <-- this is not OS independent
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

    std::vector<boost::filesystem::path> imagePaths;
    std::string extension = ".jpg";
    std::string dir = "../resources/frames/anni009/";

    boost::filesystem::recursive_directory_iterator rdi(dir);
    boost::filesystem::recursive_directory_iterator end_rdi;
    for (; rdi != end_rdi; rdi++) {
        if (extension.compare(rdi->path().extension().string()) == 0) {
            imagePaths.push_back(rdi->path());
        }
    }

    // sort according to int number of frame
    std::sort(imagePaths.begin(), imagePaths.end(), [](boost::filesystem::path aPath, boost::filesystem::path bPath) {
        std::string a = aPath.filename().string();
        std::string b = bPath.filename().string();
        return std::stoi(a) <= std::stoi(b);
    });

    std::vector<std::string> imageStrPaths;
    imageStrPaths.reserve(imagePaths.size());
    for (auto img : imagePaths)
        imageStrPaths.push_back(img.string());

    return imageStrPaths;
}

/**
 * 3.
 * Given two images, compute the differences in 8 * 8 * 8 histogram bins.
 */
Features buildHistogramDifferences(std::vector<std::string> &imagePaths, std::vector<sbd::GoldStandardElement> &goldStandard) {
    printf("Building histogram differences.\n");

    Histogram histBuilder(2);
    std::cout << "Reading " << imagePaths.size() << " images .." << std::endl;
    assert(imagePaths.size() % 2 == 0);

    cv::Mat diffs;
    cv::Mat golds;

    int tenPercent = imagePaths.size() / 10;
    for (int i = 0; i < imagePaths.size() - 1; i += 1) {
        if (i % tenPercent == 0)
            std::cout << (i / tenPercent * 10) << "% ";
        cv::Mat image1 = cv::imread(imagePaths[i], CV_LOAD_IMAGE_COLOR);
        cv::Mat image2 = cv::imread(imagePaths[i + 1], CV_LOAD_IMAGE_COLOR);
        assert(image1.total() > 0);
        assert(image2.total() > 0);

        float gold = findGold(imagePaths[i], imagePaths[i + 1], goldStandard);

        cv::Mat hist1 = histBuilder.buildHistogram(image1);
        cv::Mat oneDimHist1 = histBuilder.convertMat(hist1);

        cv::Mat hist2 = histBuilder.buildHistogram(image2);
        cv::Mat oneDimHist2 = histBuilder.convertMat(hist2);

        cv::Mat diff = oneDimHist1 - oneDimHist2;

//        Histogram::displayHistogram(hist1);
//        std::cout << "diff = " << diff << std::endl;

        diffs.push_back(diff);
        golds.push_back(gold);
    }
    std::cout << std::endl;
    Features features = { golds, diffs };
    return features;
}

/**
 * 3.1
 * For the two given files find out the gold standard, i.e. whether it is a CUT or not.
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
 * Trains the SVM with the histogram differences and the gold standard.
 */

SVMLearner* trainSVM(Features &trainSet) {
    printf("Training SVM.\n");
    // Set up training data

    cv::Mat trainMat = trainSet.values;
    cv::Mat labelsMat = trainSet.classes;

//    std::cout << trainMat << std::endl;
//    for (auto it = trainMat.begin<float>(); it != trainMat.end<float>(); it++)
//      if (*it != 0)
//        printf("%f,", *it);

    assert(trainMat.isContinuous());

    SVMLearner *svm = new SVMLearner();
    svm->train(trainMat, labelsMat);
    //svm->plotDecisionRegions();

    return svm;
}

/**
 * 5.
 * Evaluate on some held-out test set.
 */
void evaluate(Features &testSet, SVMLearner *learner) {
    printf("Evaluating on test set ...\n");

    cv::Mat testMat = testSet.values;
    cv::Mat labelsMat = testSet.classes;

    int tp = 0, fp = 0, tn = 0, fn = 0;

    for (int rowIndex = 0; rowIndex < testMat.rows; rowIndex++) {
        cv::Mat mTest = testMat.row(rowIndex);
        float predicted = learner->predict(mTest);
        float actual = labelsMat.at<float>(rowIndex, 0);
        if (predicted == actual) {
            (actual && ++tp) || tn++;
        } else {
            (actual && ++fn) || fp++;
        }
//        printf("Predicted %f   Actual: %f\n", predicted, actual);
    }

    float precision = (float) tp / (tp + fp);
    float recall = (float) tp / (tp + fn);
    float f1 = 2 * precision * recall / (precision + recall);
    printf("TP: %i FP: %i TN: %i FN: %i\n", tp, fp, tn, fn);
    printf("Precision:\t%.2f\n", precision);
    printf("Recall:\t%.2f\n", recall);
    printf("F1:\t%.2f\n", f1);
}
