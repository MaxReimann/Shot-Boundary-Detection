
#include "merger.hpp"

vector<short> sbd::MajorityVotingDiagonallyMerger::mergeSequencePredictions(
        vector<vector<short>> sequencePredictions) {
    return std::vector<short>();
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
