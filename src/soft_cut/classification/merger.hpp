#pragma once

#include "../../forwarddeclarations.hpp"

using std::vector;

namespace  sbd {
    class Merger {
    public:
        virtual vector<short> mergeSequencePredictions(vector<vector<short>>) = 0;
        // give a short name for this merge strategy
        virtual std::string name() = 0;
    };


    class MajorityVotingDiagonallyMerger : public Merger {
        virtual vector<short> mergeSequencePredictions(vector<vector<short>>) override;
        virtual std::string name() override { return "Majority-Voting-Diagonally"; }

        vector<short> getVotesForFrame(int frameNr, int sequenceSize, int videoLength, vector<vector<short>> sequencePredictions);
    };

    class TakeFirstMerger : public Merger {
        virtual vector<short> mergeSequencePredictions(vector<vector<short>>) override;
        virtual std::string name() override { return "Take-First"; }
    };

    class TakeLastMergerSequence : public Merger {
        virtual vector<short> mergeSequencePredictions(vector<vector<short>>) override;
        virtual std::string name() override { return "Take-Last-Sequence"; }
    };

    class TakeLastMergerFrame : public Merger {
        virtual vector<short> mergeSequencePredictions(vector<vector<short>>) override;
        virtual std::string name() override { return "Take-Last-Frame"; }
    };
}