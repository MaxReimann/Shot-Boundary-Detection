#pragma once

#include <src/soft_cut/io/file_writer.hpp>
#include "../forwarddeclarations.hpp"

namespace sbd
{

    struct Sequence {
        std::vector<std::string> frames;
        int clazz;
    };

    struct SequenceBatch {
        std::vector<cv::Mat> frames;
        std::vector<int> labels;

        ~SequenceBatch() {
            frames.clear();
            labels.clear();
        }
    };

    class SoftCutMain {
    public:
        int main(po::variables_map flagArgs, std::map<std::string, std::string> inputArguments);
        void writePrediction(std::vector<sbd::Sequence> sequences,
                             std::vector<float> predictions,
                             int i, int sequenceSize,
                             sbd::FileWriter &writer);
        static SequenceBatch getSequenceBatch(std::vector<Sequence> sequences, int start, int nrSequences);

    };
}