#include "gap_filler.hpp"

std::vector<short> GapFiller::fillGaps(std::vector<short> framePredictions, int maxGapsize) {
    std::vector<short> filledPredictions;

    int i = 0;
    while (i < framePredictions.size()) {
        // if cut is predicted, just keep that
        if (framePredictions[i]) {
            filledPredictions.push_back((short)1);
            i++;
        } else {
            int gapsize = 1;
            while ((i + gapsize) < framePredictions.size() && !framePredictions[i + gapsize]) {
                gapsize++;
            }
            
            if (gapsize <= maxGapsize) {
                // fill the gap, since it is small enough
                for (int j = 0; j < gapsize; j++) {
                    filledPredictions.push_back((short)1);
                }
            } else {
                // the gap is too big to be considered a gap
                for (int j = 0; j < gapsize; j++) {
                    filledPredictions.push_back((short)0);
                }
            }
            i += gapsize;
        }
    }

    return filledPredictions;
}