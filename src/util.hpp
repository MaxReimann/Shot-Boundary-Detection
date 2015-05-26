#pragma once

#include "forwarddeclarations.hpp"
#include "main.hpp"


namespace sbd {
	  void splitTrainTestSets(Features &input, float trainTestRatio, Features &trainSet, Features &testSet);
	  bool findGold(std::string path1, std::string path2, std::unordered_set<sbd::GoldStandardElement> &golds);
	  std::vector<float> getAbsChanges(cv::Mat);
	  void drawAbsChanges(std::vector<float>, cv::Mat);
}