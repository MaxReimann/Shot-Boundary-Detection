#pragma once

#include "forwarddeclarations.hpp"

typedef struct {
    std::vector<int> classes;
    std::vector<cv::Mat> values;
} Features;

std::vector<sbd::GoldStandardElement> readGoldStandard();
std::vector<std::string> getFileNames();
Features buildHistogramDifferences(std::vector<std::string> &imagePaths, std::vector<sbd::GoldStandardElement> &gold);
bool findGold(std::string path1, std::string path2, std::vector<sbd::GoldStandardElement> &gold);
void trainSVM(Features &features);
void evaluate();


