#pragma once

#include <src/soft_cut/io/file_writer.hpp>
#include "../forwarddeclarations.hpp"

namespace sbd
{

    struct Sequence {
        std::vector<std::string> frames;
        std::vector<int> clazzes;
        int clazz;
    };

    struct Video {
        std::string videoName;
        std::vector<Sequence> sequences;
    };

    struct SequenceBatch {
        std::vector<cv::Mat> frames;
        std::vector<int> labels;
        // The last batch usually cannot fill a complete batch size. Therefore we fill the remaining elements with the
        // last element repeatedly. This variable stores, how many elements in the sequence batch are actually relevant
        // elements.
        int relevantSize;

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
        std::vector<Softcut> mergeDetectedSequences(std::vector<Sequence> sequences, int sequenceSize);

        SequenceBatch getSequenceBatch(std::vector<Sequence> sequences, int start);

        SoftCutMain() : size(227, 277) {}

        void findSoftCuts();

        void processVideo(Video& video, CaffeClassifier&, std::vector<float>& predictions);

    protected:
        // Caffe parameters
        std::string preModel = "/home/pva_t1/Shot-Boundary-Detection/nets/snapshots/_iter_110000.caffemodel";
        std::string protoFile = "/home/pva_t1/Shot-Boundary-Detection/nets/deploy.prototxt";

        bool useCPU = true;
        cv::Size size;
        int channels = 3;
        bool isDebug = true;
        std::string resultLayer = "argmax";
        std::string dataLayer = "data";
        int batchSize = 70;
        int nrClasses = 2;

        // programm parameters
        int sequenceSize = 10;
        int sequenceBatchSize = batchSize / sequenceSize;
        std::string txtFile = "/opt/data_sets/video_sbd_dataset/frames/test_test.txt"; // TODO adapt to correct file
        std::string outputFile = "/home/pva_t1/Shot-Boundary-Detection/resources/predictions";
    };
}
