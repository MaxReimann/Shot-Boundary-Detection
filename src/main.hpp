#pragma once

#include "forwarddeclarations.h"

void getFileNames();
cv::Mat readImages();
void buildHistogramDifferences(cv::Mat);
void trainSVM();
void readGoldStandard();
void evaluate();


