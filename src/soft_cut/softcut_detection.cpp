#include "softcut_detection.hpp"
#ifndef _WIN32
#include <glog/logging.h>
#endif
#include <src/soft_cut/classification/caffe_classifier.hpp>
#include "src/soft_cut/io/file_writer.hpp"
#include <boost/format.hpp>
#include <src/soft_cut/io/file_reader.hpp>
#include <algorithm>
#include <src/evaluation/evaluation.hpp>

void wrongUsage();

using namespace sbd;

int SoftCutMain::main(po::variables_map flagArgs, std::map<std::string, std::string> inputArguments) {
#ifndef _WIN32
    // Disable caffe logging (1: log warnings, 3: log nothing)
    FLAGS_minloglevel = 1;


    SoftCutMain softcut = SoftCutMain();
    softcut.findSoftCuts();
#endif
    return 0;
}

void SoftCutMain::findSoftCuts() {

    // TODO
    // (1) Evaluierung einbauen: Frame-Level, Sequence-Level, Video-Level
    // (2) Mergen von 10er Sequenzen die einen Soft-Cut darstellen. -> Macht Felix
    //     Beispiel: 5 Sequenzen wurden als Soft Cut erkannt, eine nicht und dann wieder 5 als Soft Cut.
    //               Die 11 Sequenzen sind wahrscheinlich ein Soft Cut.


    // 1. Get all videos with frame paths and class label
    std::vector<Video> videos;
    FileReader::load(txtFile, sequenceSize, videos);

    // 2. Initialize classifier
    std::cout << "Initialize classifier ..." << std::endl;
    CaffeClassifier classifier(useCPU, preModel, protoFile, size, channels, isDebug);

    // 3. Predict and evaluating all videos
    Evaluation evaluation("Sequence-Level-Evaluation", nrClasses);
    for (auto video : videos) {
        std::cout << "Predicting video " << video.videoName << std::endl;
        std::vector<float> predictions;
        processVideo(video, classifier, predictions);
        assert(predictions.size() == video.sequences.size());
        for (int i = 0; i < predictions.size(); i++) {
            float actual = static_cast<float>(video.sequences[i].clazz);
            evaluation.prediction((int) predictions[i], (int) actual);
        }
    }
    std::cout << evaluation.summaryString() << std::endl;
}

void SoftCutMain::processVideo(Video& video, CaffeClassifier& classifier, std::vector<float>& predictions) {
    std::cout << "Predicting " << video.sequences.size() << " sequences" << std::endl;

    std::string outputFileName = outputFile + "_" + video.videoName + ".txt";
    FileWriter writer(outputFileName);
    Evaluation videoFrameEvaluation(video.videoName, nrClasses);

    for (int i = 0; i < video.sequences.size(); i += sequenceBatchSize) {
        std::cout << (i * 100) / video.sequences.size() << "% " << std::flush;

        // get data for the batch of videos
        SequenceBatch sequenceBatch = getSequenceBatch(video.sequences, i);

        // get prediction for frames
        std::vector<float> framePredictions;
        classifier.predict(sequenceBatch.frames, sequenceBatch.labels, resultLayer, dataLayer, framePredictions);
        framePredictions = std::vector<float>(framePredictions.begin(), framePredictions.begin() + sequenceBatch.relevantSize);
        assert(framePredictions.size() == sequenceBatch.relevantSize);

        for (int j = 0; j < framePredictions.size(); j++) {
            float actual = static_cast<float>(sequenceBatch.labels[j]);
            videoFrameEvaluation.prediction((int) framePredictions[j], (int) actual);
            if (j % sequenceSize == sequenceSize - 1)
                predictions.push_back(framePredictions[j]);
        }

        // write predictions
        writePrediction(video.sequences, framePredictions, i, sequenceSize, writer);

        framePredictions.clear();

    }
    std::cout << std::endl;

    std::cout << videoFrameEvaluation.summaryString() << std::endl;
    std::cout << "Wrote prediction to " << outputFileName << std::endl;
    writer.close();

}

void SoftCutMain::writePrediction(std::vector<Sequence> sequences,
    std::vector<float> predictions,
    int i, int sequenceSize,
    FileWriter &writer) {

    for (int k = 0; k < predictions.size(); k++) {
        Sequence sequence = sequences[i + k / sequenceSize];

        int pred = (int)predictions[k];
        int actual = sequence.clazzes[k % sequenceSize];
        assert(sequence.clazzes.size() == sequenceSize);

        boost::format line("Frame: %s Predicted: %-3d Actual: %-3d");
        line % sequence.frames[k % sequenceSize];
        line % pred;
        line % actual;
        writer.writeLine(line.str());
    }
}

SequenceBatch SoftCutMain::getSequenceBatch(std::vector<Sequence> sequences, int start) {
    std::vector<cv::Mat> frames;
    std::vector<int> labels;

    for (int i = start; i < start + sequenceBatchSize; i++) {
        int index = std::min(i, static_cast<int>(sequences.size()) - 1);
        Sequence sequence = sequences[index];

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
            labels.push_back(sequence.clazzes[j]);
        }
    }

    SequenceBatch sequenceBatch;
    sequenceBatch.frames = frames;
    sequenceBatch.labels = labels;
    if (start + sequenceBatchSize > sequences.size()) { 
        sequenceBatch.relevantSize = batchSize - (sequenceSize * (start + sequenceBatchSize - sequences.size()));
    } else {
        sequenceBatch.relevantSize = batchSize;
    }
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

void wrongUsageSoftCut() {
    std::cout << "Usage: sbd --soft_cut" << std::endl;
#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif
    exit(1);
}


