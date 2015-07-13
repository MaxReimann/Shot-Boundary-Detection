
#include "merger.hpp"

vector<short> sbd::MajorityVotingDiagonallyMerger::mergeSequencePredictions(
        vector<vector<short>> sequencePredictions) {
    int sequenceSize = static_cast<int>(sequencePredictions[0].size());
    int videoLength = static_cast<int>(sequencePredictions.size());

    vector<short> framePredictions;

    for (auto frameNr = 0; frameNr < videoLength; frameNr++) {
        vector<short> votes = getVotesForFrame(frameNr, sequenceSize, videoLength, sequencePredictions);

        auto sum = 0;
        for (auto &vote : votes) {
            sum += vote;
        }
        auto finalVote = static_cast<float>(sum) / votes.size() > 0.5 ? 1 : 0;
        framePredictions.push_back(static_cast<const short &>(finalVote));
    }

    return framePredictions;
}

vector<short> sbd::MajorityVotingDiagonallyMerger::getVotesForFrame(int frameNr, int sequenceSize, int videoLength,
                                                                    vector<vector<short>> sequencePredictions) {
    vector<short> votes;
    // the first videos have fewer votes, hence the min
    for (auto j = 0; j < std::min(frameNr + 1, sequenceSize); j++) {
        auto videoIndex = frameNr - j;
        auto frameIndex = j;
//        std::cout << "Trying to access [" << videoIndex << ", " << frameIndex << "].";
        votes.push_back(sequencePredictions[videoIndex][j]);
    }
    return votes;
}

vector<short> sbd::TakeFirstMerger::mergeSequencePredictions(
        vector<vector<short>> sequencePredictions) {
    vector<short> framePredictions;
    for (auto sequence : sequencePredictions) {
        framePredictions.push_back(sequence[0]);
    }
    return framePredictions;
}

vector<short> sbd::TakeLastMergerSequence::mergeSequencePredictions(
        vector<vector<short>> sequencePredictions) {
    vector<short> framePredictions;
    for (auto sequence : sequencePredictions) {
        framePredictions.push_back(sequence.back());
    }
    return framePredictions;
}

vector<short> sbd::TakeLastMergerFrame::mergeSequencePredictions(
        vector<vector<short>> sequencePredictions) {
    vector<short> framePredictions;
    int sequenceSize = static_cast<int>(sequencePredictions[0].size());
    int videoLength = static_cast<int>(sequencePredictions.size());

    for (auto frameNr = 0; frameNr < videoLength; frameNr++) {
        if (frameNr < sequenceSize - 1) {
            framePredictions.push_back(sequencePredictions[0][frameNr]);
        } else {
            framePredictions.push_back(sequencePredictions[frameNr - (sequenceSize - 1)][sequenceSize - 1]);
        }
    }

    return framePredictions;
}
