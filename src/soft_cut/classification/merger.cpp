
#include "merger.hpp"

vector<short> sbd::MajorityVotingDiagonallyMerger::mergeSequencePredictions(
        vector<vector<short>> sequencePredictions) {
    int sequenceSize = static_cast<int>(sequencePredictions[0].size());
    int videoLength = static_cast<int>(sequencePredictions.size()) + sequenceSize - 1;

    vector<short> framePredictions;

    for (auto videoNr = 0; videoNr < videoLength; videoNr++) {
        vector<short> votes;
        // the first videos have fewer votes, hence the min
        for (auto j = 0; j < std::min(videoNr + 1, sequenceSize); j++) {
            // the last videos also have fewer videos, therefore we check here:
            if (videoNr - j < videoLength)
                votes.push_back(sequencePredictions[videoNr - j][j]);
        }

        auto sum = 0;
        for (auto &vote : votes) {
            sum += vote;
        }
        auto finalVote = static_cast<float>(sum) / votes.size() > 0.5 ? 1 : 0;
        framePredictions.push_back(static_cast<const short &>(finalVote));
    }

    return framePredictions;
}

vector<short> sbd::TakeFirstMerger::mergeSequencePredictions(
        vector<vector<short>> sequencePredictions) {
    vector<short> framePredictions;
    for (auto sequence : sequencePredictions) {
        framePredictions.push_back(sequence[0]);
    }
    return framePredictions;
}

vector<short> sbd::TakeLastMerger::mergeSequencePredictions(
        vector<vector<short>> sequencePredictions) {
    vector<short> framePredictions;
    for (auto sequence : sequencePredictions) {
        framePredictions.push_back(sequence.back());
    }
    return framePredictions;
}
