#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "main.hpp"
#include "histogram/histogram.hpp"
#include "gold_standard/file_reader.hpp"
#include "gold_standard/gold_standard_element.hpp"
#include "gold_standard/gold_standard_statistic.hpp"
#include "svm/svm.hpp"
#include "util.hpp"

using namespace sbd;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: sbd data_folder" << std::endl;
        std::cout << "  data_folder: Folder for the images and the truth data. Must contain the placeholder [type], which will be replaced by 'frames' or 'truth'" << std::endl;
        std::cout << "               For local execution, just set this to '../resources/[type]/'" << std::endl;
        exit(1);
    }
    std::string dataFolder(argv[1]);

    GoldStandardStatistic::create(dataFolder);

    std::vector<sbd::GoldStandardElement> gold = readGoldStandard(dataFolder);
    std::vector<std::string> imagePaths = getFileNames(dataFolder);
    Features features = buildHistogramDifferences(imagePaths, gold);

    Features trainSet, testSet;
    splitTrainTestSets(features, 0.7, trainSet, testSet);
    SVMLearner* learner = trainSVM(trainSet);
    evaluate(testSet, learner);

    // wait for key, so we can read the console output
#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif
    return 0;
}

/**
 * 1.
 * Reads the gold standard and returns it in some easy format.
 */
std::vector<sbd::GoldStandardElement> readGoldStandard(std::string dataFolder) {
    printf("Reading gold standard.\n");

    std::string truthFolder = boost::replace_first_copy(dataFolder, "[type]", "truth");

    FileReader fileReader;
    std::vector<sbd::GoldStandardElement> goldStandard = fileReader.readDir(truthFolder.c_str(), true);

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
std::vector<std::string> getFileNames(std::string dataFolder) {
    printf("Getting frame file names.\n");

    std::vector<boost::filesystem::path> imagePaths;
    std::string extension = ".jpg";
    std::string framesFolder = boost::replace_first_copy(dataFolder, "[type]", "frames");

    boost::filesystem::recursive_directory_iterator rdi(framesFolder);
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

    Histogram histBuilder(8);
    std::cout << "Reading " << imagePaths.size() << " images .." << std::endl;

    cv::Mat diffs;
    cv::Mat golds;

    int tenPercent = imagePaths.size() / 10;
    for (int i = 0; i < imagePaths.size() - 1; i += 1) {
        if (i % tenPercent == 0)
            std::cout << (i / tenPercent * 10) << "% " << std::flush;
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
    std::string videoName1 = boost::filesystem::path(path1).parent_path().stem().string();
    std::string videoName2 = boost::filesystem::path(path2).parent_path().stem().string();
    std::string frameNr1 = boost::filesystem::path(path1).stem().string();
    std::string frameNr2 = boost::filesystem::path(path2).stem().string();

    for (int i = 0; i < gold.size(); i++) {
        if (videoName1 == gold[i].name &&
                videoName2 == gold[i].name &&
                frameNr1 == std::to_string(gold[i].startFrame) &&
                frameNr2 == std::to_string(gold[i].endFrame))
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

    std::cout << "Training on " << cv::sum(labelsMat)[0] << "/" << labelsMat.size().height << " positive instances." << std::endl;

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
    std::cout << "Evaluating on " << cv::sum(labelsMat)[0] << "/" << labelsMat.size().height << " positive instances." << std::endl;

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

    float precision = static_cast<float>(tp) / (tp + fp);
    float recall    = static_cast<float>(tp) / (tp + fn);
    float f1        = 2 * precision * recall / (precision + recall);
    float accuracy  = static_cast<float>(tp + tn) / (tp + tn + fp + fn);

    std::cout.precision(2);
    printf("TP: %i FP: %i TN: %i FN: %i\n", tp, fp, tn, fn);
    std::cout << std::setw(11) << "Precision: " << precision << std::endl;
    std::cout << std::setw(11) << "Recall: "    << recall << std::endl;
    std::cout << std::setw(11) << "F1: "        << f1 << std::endl;
    std::cout << std::setw(11) << "Accuracy: "  << accuracy << std::endl;
}
