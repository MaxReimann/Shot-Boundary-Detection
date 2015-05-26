#include "util.hpp"
#include <numeric>
#include <boost/filesystem.hpp>
#include <src/gold_standard/gold_standard_element.hpp>
#include <numeric>

using namespace sbd;

// split the feature vector in train and test set by trainSetRatio (e.g. 0.7 for 70% trainingset and 30% test)
void sbd::splitTrainTestSets(Features &input, float trainSetRatio, Features &trainSet, Features &testSet)
{
    int cols = input.values.cols;
    int rows = input.values.rows;
    assert(cols > 0);
    cv::Mat trainValues = cv::Mat(0, cols, CV_32FC1);
    cv::Mat testValues  = cv::Mat(0, cols, CV_32FC1);
    cv::Mat trainLabels = cv::Mat(0, 1, CV_32FC1);
    cv::Mat testLabels  = cv::Mat(0, 1, CV_32FC1);

    // create an array containing the random row order, so we can shuffle the train/test set order
    std::vector<int> randomRows(rows);
    std::iota(randomRows.begin(), randomRows.end(), 0); // Fill with 0, 1, ..., n-1
    srand(1599);
    std::random_shuffle(randomRows.begin(), randomRows.end());

    for (int i = 0; i < rows; i++) {
        float currentRatio = i / static_cast<float>(rows);
        int randomRow = randomRows[i];
        if (currentRatio < trainSetRatio) {
            trainValues.push_back(input.values.row(randomRow));
            trainLabels.push_back(input.classes.row(randomRow));
        }
        else {
            testValues.push_back(input.values.row(randomRow));
            testLabels.push_back(input.classes.row(randomRow));
        }
    }

    trainSet.values = trainValues;
    trainSet.classes = trainLabels;
    testSet.values = testValues;
    testSet.classes = testLabels;
}


// For the two given files find out the gold standard, i.e. whether it is a CUT or not.
bool sbd::findGold(std::string path1, std::string path2, std::unordered_set<sbd::GoldStandardElement> &golds) {
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