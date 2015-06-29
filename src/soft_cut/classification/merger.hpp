#pragma once

#include "../../forwarddeclarations.hpp"

using std::vector;

namespace  sbd {
    class Merger {
    public:
        virtual vector<short> mergeSequencePredictions(vector<vector<short>>) = 0;
    };


    class MajorityVotingDiagonallyMerger : public Merger {
        virtual vector<short> mergeSequencePredictions(vector<vector<short>>) override;
    };
}