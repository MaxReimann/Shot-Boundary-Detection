#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include "main.hpp"
#include "histogram/histogram.hpp"
#include "gold_standard/file_reader.hpp"
#include "gold_standard/gold_standard_element.hpp"
#include "gold_standard/gold_standard_statistic.hpp"
#include "svm/svm.hpp"
#include "util.hpp"
#include <random>
#include <iostream>
#include <omp.h>

using namespace sbd;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: sbd data_folder" << std::endl;
        std::cout << "  data_folder: Folder for the images and the truth data. Must contain the placeholder [type], which will be replaced by 'frames' or 'truth'" << std::endl;
        std::cout << "               For local execution, just set this to '../resources/[type]/'" << std::endl;
        exit(1);
    }
    std::string dataFolder(argv[1]);

//    GoldStandardStatistic::create(dataFolder);
    Features features;
    std::unordered_set<sbd::GoldStandardElement> gold = readGoldStandard(dataFolder);
    std::string histogramCachePath = "../resources/differenceHistograms.yaml";
    
    cv::FileStorage fs;
    if (!boost::filesystem::exists(histogramCachePath))
    {
        std::vector<std::string> imagePaths = getFileNames(dataFolder);
        features = buildHistogramDifferences(imagePaths, gold);
        std::cout << "Caching built histograms." << std::endl;
        fs.open(histogramCachePath, cv::FileStorage::WRITE);
        fs << "Histograms" << features.values;
        fs << "Labels" << features.classes;
    }
    else
    {
        std::cout << "Using cached histogram differences." << std::endl;
        fs.open(histogramCachePath, cv::FileStorage::READ);
        fs["Histograms"] >> features.values;
        fs["Labels"] >> features.classes;
    }

    fs.release();

    Features trainSet, testSet;
    splitTrainTestSets(features, 0.7, trainSet, testSet);
    cv::Ptr<SVMLearner> learner = trainSVM(trainSet);
    evaluate(testSet, learner);

    // wait for key, so we can read the console output
#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif
    return 0;
}

void createRandomTransition(std::vector<sbd::GoldStandardElement> &gold,
    std::vector<std::string> imagePaths) {
    
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> goldSizeDist(0, gold.size() - 2);

    int cutStart1 = goldSizeDist(mt);
    int cutStart2 = goldSizeDist(mt);

    bool retry = false;
    retry = retry || cutStart1 == cutStart2;
    if (retry) {
        std::cout << "same start frames. retry" << std::endl;
        return createRandomTransition(gold, imagePaths);
    }

    int sequence1Start = gold[cutStart1].endFrame;
    int sequence1End = gold[cutStart1 + 1].startFrame;

    int sequence2Start = gold[cutStart2].endFrame;
    int sequence2End = gold[cutStart2 + 1].startFrame;

    std::uniform_int_distribution<int> transitionLengthDist(10, 25);
    int transitionLength = transitionLengthDist(mt);


    retry = retry || (sequence1End - sequence1Start) < transitionLength;
    retry = retry || (sequence2End - sequence2Start) < transitionLength;

    std::cout << transitionLength << std::endl;
    std::cout << sequence1Start << " " << sequence1End << std::endl;

    if (retry) {
        std::cout << "retry";
        createRandomTransition(gold, imagePaths);
        return;
    }

    std::uniform_int_distribution<int> startFrame1Dist(
        sequence1Start, sequence1End - transitionLength);
    std::uniform_int_distribution<int> startFrame2Dist(
        sequence2Start, sequence2End - transitionLength);

    int startFrame1 = startFrame1Dist(mt);
    int startFrame2 = startFrame2Dist(mt);

    for (size_t i = 0; i < transitionLength; i++) {

        cv::Mat image1 = cv::imread("../resources/frames/senses111/" + std::to_string(startFrame1 + i) + ".jpg", CV_LOAD_IMAGE_COLOR);
        cv::Mat image2 = cv::imread("../resources/frames/senses111/" + std::to_string(startFrame2 + i) + ".jpg", CV_LOAD_IMAGE_COLOR);
     
        cv::Mat result;
        float alpha = static_cast<float>(i) / transitionLength;
        float beta = 1 - alpha;
        cv::addWeighted(image1, alpha, image2, beta, 0.0, result);

        std::cout << "writing file" << startFrame1 << startFrame2 << std::endl;
        bool b =  cv::imwrite(
            "../output/sequence" + std::to_string(startFrame1) + "-" + std::to_string(startFrame2)
            + std::to_string(i) + ".jpg",
        result);
    }

}

/**
 * 1.
 * Reads the gold standard and returns it in some easy format.
 */
std::unordered_set<sbd::GoldStandardElement> readGoldStandard(std::string dataFolder) {
    printf("Reading gold standard.\n");

    std::string truthFolder = boost::replace_first_copy(dataFolder, "[type]", "truth");
    std::cout << "Reading truth from " << truthFolder << std::endl;

    FileReader fileReader;
    std::unordered_set<sbd::GoldStandardElement> goldStandard = fileReader.readDir(truthFolder.c_str(), true);

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
    std::cout << "Reading frames from " << framesFolder << std::endl;

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
Features buildHistogramDifferences(std::vector<std::string> &imagePaths, std::unordered_set<sbd::GoldStandardElement> &goldStandard) {
    printf("Building histogram differences.\n");

    Histogram histBuilder(8);
    std::cout << "Reading " << imagePaths.size() << " images .." << std::endl;

    cv::Mat diffs;
    cv::Mat golds;

    int tenPercent = imagePaths.size() / 10;
    std::cout << "0% " << std::flush;
#pragma omp parallel for
    for (int i = 0; i < imagePaths.size() - 1; i += 1) {
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

        //Histogram::displayHistogram(hist1);
        //std::cout << "diff = " << diff << std::endl;
#pragma omp critical
        {
            diffs.push_back(diff);
            golds.push_back(gold);

            if (diffs.rows % tenPercent == 0)
                std::cout << (diffs.rows / tenPercent * 10) << "% " << std::flush;
        }
    }
    std::cout << std::endl;
    Features features = { golds, diffs };

    return features;
}

/**
 * 3.1
 * For the two given files find out the gold standard, i.e. whether it is a CUT or not.
 */
bool findGold(std::string path1, std::string path2, std::unordered_set<sbd::GoldStandardElement> &golds) {
    std::string videoName1 = boost::filesystem::path(path1).parent_path().stem().string();
    std::string videoName2 = boost::filesystem::path(path2).parent_path().stem().string();
    std::string frameNr1 = boost::filesystem::path(path1).stem().string();
    std::string frameNr2 = boost::filesystem::path(path2).stem().string();

    GoldStandardElement gold(videoName1, videoName2, std::stoi(frameNr1), std::stoi(frameNr2));

//    std::cout << videoName1 << "-" << videoName2 << "-" << frameNr1 << "-" << frameNr2 << std::endl;
//    for (int i = 0; i < gold.size(); i++) {
//        if (videoName1 == gold[i].name &&
//                videoName2 == gold[i].name &&
//                frameNr1 == std::to_string(gold[i].startFrame) &&
//                frameNr2 == std::to_string(gold[i].endFrame))
//            return true;
//    }
//    return false;
    return golds.find(gold) != golds.end();
}

/**
 * 4.
 * Trains the SVM with the histogram differences and the gold standard.
 */

cv::Ptr<sbd::SVMLearner> trainSVM(Features &trainSet) {
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

    cv::Ptr<SVMLearner> svm = new SVMLearner();
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
