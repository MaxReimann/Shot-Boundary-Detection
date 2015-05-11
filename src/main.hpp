#pragma once

#include "forwarddeclarations.hpp"

typedef struct {
    cv::Mat classes;
    cv::Mat values;
} Features;

std::vector<sbd::GoldStandardElement> readGoldStandard(std::string);
std::vector<std::string> getFileNames(std::string);
Features buildHistogramDifferences(std::vector<std::string> &imagePaths, std::vector<sbd::GoldStandardElement> &gold);
bool findGold(std::string path1, std::string path2, std::vector<sbd::GoldStandardElement> &gold);
sbd::SVMLearner* trainSVM(Features &trainSet);
void evaluate(Features &testSet, sbd::SVMLearner *learner);

