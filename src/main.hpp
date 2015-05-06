#pragma once

#include "forwarddeclarations.hpp"

std::vector<sbd::GoldStandardElement> readGoldStandard();
void getFileNames();
cv::Mat readImages();
void buildHistogramDifferences(cv::Mat);
void trainSVM();
void evaluate();


