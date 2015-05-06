#pragma once

#include "forwarddeclarations.hpp"

std::vector<sbd::GoldStandardElement> readGoldStandard();
std::vector<std::string> getFileNames();
std::vector<cv::MatND> buildHistogramDifferences(std::vector<std::string>);
void trainSVM();
void evaluate();


