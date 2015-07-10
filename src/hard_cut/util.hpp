#pragma once

#include "../forwarddeclarations.hpp"
#include "hardcut_detection.hpp"
#include <random>
#include <iterator>

namespace sbd {
	  void splitTrainTestSets(Features &input, float trainTestRatio, Features &trainSet, Features &testSet);
	  bool findGold(std::string path1, std::string path2, std::unordered_set<sbd::GoldStandardElement> &golds);
      void writeVisualizationData(std::vector<std::string> &imagePaths, std::vector<float> diffs, cv::Mat& gold, std::vector<float> predictions);
}

