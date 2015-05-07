#pragma once

#include "forwarddeclarations.hpp"

typedef struct {
    std::vector<bool> classes;
    std::vector<cv::Mat> values;
} Features;

std::vector<sbd::GoldStandardElement> readGoldStandard();
std::vector<std::string> getFileNames();
Features buildHistogramDifferences(std::vector<std::string>, std::vector<sbd::GoldStandardElement>);
bool findGold(std::string path1, std::string path2, std::vector<sbd::GoldStandardElement>);
void trainSVM(Features);
void evaluate();


