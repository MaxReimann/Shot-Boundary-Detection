#pragma once

#include "forwarddeclarations.hpp"

std::vector<sbd::GoldStandardElement> readGoldStandard();
std::vector<std::string> getFileNames();
std::vector<cv::MatND> buildHistogramDifferences(std::vector<std::string>, std::vector<sbd::GoldStandardElement>);
bool findGold(std::string path1, std::string path2, std::vector<sbd::GoldStandardElement>);
void trainSVM();
void evaluate();


