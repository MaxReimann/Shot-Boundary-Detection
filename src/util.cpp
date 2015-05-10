#include "util.hpp"

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
    int randomRows[rows];
    for (int i = 0; i < rows; i++)
        randomRows[i] = i;
    srand(1599);
    std::random_shuffle(&randomRows[0], &randomRows[rows]);

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
