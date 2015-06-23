#pragma once

#include <src/soft_cut/io/file_writer.hpp>
#include "../forwarddeclarations.hpp"

namespace sbd
{

    struct Sequence {
        std::vector<std::string> frames;
        int clazz;
        std::string videoName;
    };

    struct SequenceBatch {
        std::vector<cv::Mat> frames;
        std::vector<int> labels;
        ~SequenceBatch() {
            frames.clear();
            labels.clear();
        }
    };

    struct Softcut {
        std::string firstFrame;
        std::string lastFrame;
        int length;
    };

    class SoftCutMain {
    public:
        int main(po::variables_map flagArgs, std::map<std::string, std::string> inputArguments);
        void writePrediction(std::vector<sbd::Sequence> sequences,
                             std::vector<float> predictions,
                             int i, int sequenceSize,
                             sbd::FileWriter &writer);
        static SequenceBatch getSequenceBatch(std::vector<Sequence> sequences, int start, int nrSequences);
        std::vector<Softcut> mergeDetectedSequences(std::vector<Sequence> sequences, int sequenceSize);

    };
}