#pragma once

#include "forwarddeclarations.hpp"
#include "main.hpp"


namespace sbd {
	  void splitTrainTestSets(Features &input, float trainTestRatio, Features &trainSet, Features &testSet);
}