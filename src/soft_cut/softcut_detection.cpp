#include "softcut_detection.hpp"
#ifndef _WIN32
#include <glog/logging.h>
#endif
#include <src/soft_cut/classification/caffe_classifier.hpp>
#include "src/soft_cut/io/file_writer.hpp"
#include <boost/format.hpp>
#include <src/soft_cut/io/file_reader.hpp>
#include <algorithm>

void wrongUsage();

using namespace sbd;

int SoftCutMain::main(po::variables_map flagArgs, std::map<std::string, std::string> inputArguments) {
    // Disable logging (1: log warnings, 3: log nothing)
#ifndef _WIN32
    FLAGS_minloglevel = 1;

    // Caffe parameters
    std::string preModel = "path_to_model";
    std::string protoFile = "path_to_deploy_prototxt";
    bool cpuSetting = false;
    cv::Size size(227, 227);
    int channels = 3;
    bool isDebug = true;
    std::string resultLayer = "argmax";
    std::string dataLayer = "data";
    int batchSize = 70;
    int nrClasses = 2;

    // programm parameters
    int sequenceSize = 10;
    int sequenceBatchSize = batchSize / sequenceSize;
    std::string txtFile = "txtFile";
    std::string outputFile = "outputFile";


    /**
    * MAIN
    */


    // 1. Get all sequences with frame paths and class label
    std::vector<Sequence> sequences;
    FileReader::load(txtFile, sequenceSize, sequences);

    if (sequences.size() % sequenceBatchSize != 0) {
        std::cout << "Number of sequences (" << sequences.size() << ") modulo " << sequenceBatchSize << " is not equal to 0!" << std::endl;
        exit(99);
    }

    // 2. Initialize classifier
    std::cout << "Initialize classifier ..." << std::endl;
    CaffeClassifier classifier(cpuSetting, preModel, protoFile, size, channels, isDebug);

    FileWriter writer(outputFile);

    // 4. Predict all sequences
    std::cout << "Predicting " << sequences.size() << " sequences ..." << std::endl;
    for (int i = 0; i < sequences.size(); i += sequenceBatchSize) {
        std::cout << (i * 100) / sequences.size() << "% " << std::flush;

        // get data for the batch of sequences
        SequenceBatch sequenceBatch = getSequenceBatch(sequences, i, sequenceBatchSize);

        // get prediction for frames
        std::vector<float> predictions;
        classifier.predict(sequenceBatch.frames, sequenceBatch.labels, resultLayer, dataLayer, predictions);

        // write predictions
        writePrediction(sequences, predictions, i, sequenceSize, writer);

        predictions.clear();
    }

    std::cout << std::endl;

    writer.close();
#endif
    return 0;
}


void SoftCutMain::writePrediction(std::vector<Sequence> sequences,
    std::vector<float> predictions,
    int i, int sequenceSize,
    FileWriter &writer) {
    for (int k = 0; k < predictions.size(); k++) {
        Sequence sequence = sequences[i + k / sequenceSize];

        int pred = (int)predictions[k];
        int actual = sequence.clazz;

        boost::format line("Frame: %s Predicted: %-3d Actual: %-3d");
        line % sequence.frames[k];
        line % pred;
        line % actual;
        writer.writeLine(line.str());
    }
}

SequenceBatch SoftCutMain::getSequenceBatch(std::vector<Sequence> sequences, int start, int nrSequences) {
    std::vector<cv::Mat> frames;
    std::vector<int> labels;

    for (int i = start; i < start + nrSequences; i++) {
        Sequence sequence = sequences[i];

        // reading frames and labels of sequence
        for (int j = 0; j < sequence.frames.size(); j++) {
            std::string frameFile = sequence.frames[j];
            cv::Mat frame = cv::imread(frameFile);

            // check if image contains data
            if (!frame.data) {
                std::cerr << "Warning: input image (" << frameFile << ") for caffe classifier is empty." << std::endl;
                exit(1);
            }

            frames.push_back(frame);
            labels.push_back(sequence.clazz);
        }
    }

    SequenceBatch sequenceBatch;
    sequenceBatch.frames = frames;
    sequenceBatch.labels = labels;
    return sequenceBatch;
}

std::vector<Softcut> mergeDetectedSequences(std::vector<Sequence> sequences, int sequenceSize) {
    std::vector<int> centers;
    std::vector<Softcut> softcuts;
    int i = 0;
    while (i < sequences.size()) {
        std::vector<std::string> mergedFrames;
        // collect sequences of detected sequemces
        while (sequences[i].clazz) {
            for (int j = 0; j < sequenceSize; j++) {
                // check if current frame already is in mergedFrames
                if (std::find(mergedFrames.begin(), mergedFrames.end(), sequences[i].frames[j]) == mergedFrames.end()) {
                    // it is not, so add it
                    mergedFrames.push_back(sequences[i].frames[j]);
                }
            }
            i++;
        }

        // mergedFrames now contains the full sequence of frames that belong to the softcut (or at least the major part of it)
        Softcut cut;
        cut.firstFrame = mergedFrames[0];
        cut.lastFrame = mergedFrames.back();
        cut.length = mergedFrames.size();
        softcuts.push_back(cut);

        i++;
    }

    return softcuts;
}

void wrongUsageSoftCut()
{
    std::cout << "Usage: sbd --soft_cut" << std::endl;
#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif
    exit(1);
}