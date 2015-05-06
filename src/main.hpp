#pragma once

#include "forwarddeclarations.hpp"

std::vector<sbd::GoldStandardElement> readGoldStandard();
std::vector<std::string> getFileNames();
cv::Mat readImages(std::vector<std::string>);
void buildHistogramDifferences(cv::Mat);
void trainSVM();
void evaluate();


