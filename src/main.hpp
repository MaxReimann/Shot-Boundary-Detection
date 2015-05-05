#pragma once

#include "forwarddeclarations.hpp"

void getFileNames();
cv::Mat readImages();
void buildHistogramDifferences(cv::Mat);
void trainSVM();
void readGoldStandard();
void evaluate();


