#pragma once

#include "../../forwarddeclarations.hpp"

namespace  sbd {
    class Merger {
    public:
        virtual std::vector<short> mergeSequencePredictions(std::vector<std::vector<short>>) = 0;
    };


    class MajorityVotingDiagonallyMerger : public Merger {
        virtual std::vector<short> mergeSequencePredictions(std::vector<std::vector<short>>) override;
    };
}