#pragma once

#include "../../forwarddeclarations.hpp"

class GapFiller {
public:
    static std::vector<short> fillGaps(std::vector<short> framePredictions, int maxGapSize);
};