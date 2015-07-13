#pragma once

#include <src/soft_cut/io/file_writer.hpp>
#include "../forwarddeclarations.hpp"

namespace sbd
{

    struct Video {
        std::string videoName;
        std::vector<short> actual;
        std::vector<std::string> frames;
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

        void writePrediction(std::string videoName, std::vector<short> actual, std::vector<short> predictions, std::vector<std::string> frames);

        SequenceBatch getSequenceBatch(Video video, int start);

        SoftCutMain() : size(227, 227) {}

        void findSoftCuts();

        void processVideo(Video& video, CaffeClassifier&, std::vector<std::vector<short>>& predictions);
        void writeVisualizationData(std::vector<Video> &videos, std::vector<std::vector<std::vector<short>>> visPredictions);
    protected:
        // Caffe parameters
        std::string preModel = "/home/pva_t1/Shot-Boundary-Detection/nets/experiments/one_lstm/_iter_40000.caffemodel";
        std::string protoFile = "/home/pva_t1/Shot-Boundary-Detection/nets/experiments/one_lstm/deploy.prototxt";

        bool useCPU = false;
        cv::Size size;
        int channels = 3;
        bool isDebug = true;
        std::string resultLayer = "argmax";
        std::string dataLayer = "data";
        int batchSize = 77;
        int nrClasses = 2;

        // programm parameters
        int sequenceSize = 11;
        int sequenceBatchSize = batchSize / sequenceSize;
        std::string txtFile = "/opt/data_sets/video_sbd_dataset/generated_soft_cuts/files-2.txt";
        std::string outputFile = "/home/pva_t1/Shot-Boundary-Detection/resources/predictions_";
    };
}
