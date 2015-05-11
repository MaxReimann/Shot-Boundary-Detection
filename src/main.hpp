#pragma once

#include "forwarddeclarations.hpp"

typedef struct {
    cv::Mat classes;
    cv::Mat values;
} Features;

std::unordered_set<sbd::GoldStandardElement> readGoldStandard(std::string);
std::vector<std::string> getFileNames(std::string);
Features buildHistogramDifferences(std::vector<std::string> &imagePaths, std::unordered_set<sbd::GoldStandardElement> &gold);
bool findGold(std::string path1, std::string path2, std::unordered_set<sbd::GoldStandardElement> &gold);
sbd::SVMLearner* trainSVM(Features &trainSet);
void evaluate(Features &testSet, sbd::SVMLearner *learner);
void createRandomTransition(std::vector<sbd::GoldStandardElement> &gold, std::vector<std::string> imagePaths);
