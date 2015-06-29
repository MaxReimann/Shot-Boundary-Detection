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
#include <src/soft_cut/classification/merger.hpp>
#include "src/soft_cut/classification/gap_filler.hpp"

void wrongUsage();

using namespace sbd;

int SoftCutMain::main(po::variables_map flagArgs, std::map<std::string, std::string> inputArguments) {
#ifndef _WIN32
    // Disable caffe logging (1: log warnings, 3: log nothing)
    FLAGS_minloglevel = 1;
#endif

    SoftCutMain softcut = SoftCutMain();
    softcut.findSoftCuts();
    return 0;
}

void SoftCutMain::findSoftCuts() {
    // 1. Get all videos with frame paths and class label
    std::vector<Video> videos;
    FileReader::load(txtFile, videos);

    // 2. Initialize classifier
    std::cout << "Initialize classifier ..." << std::endl;
    CaffeClassifier classifier(useCPU, preModel, protoFile, size, channels, isDebug);

    for (auto video : videos) {
        // 3. Predict all videos
        std::cout << "Predicting video " << video.videoName << std::endl;
        std::vector<std::vector<short>> sequencePredictions;
        processVideo(video, classifier, sequencePredictions);

        std::vector<short> actual = video.actual;

        std::vector<Merger*> mergeStrategies = {
            // new MajorityVotingDiagonallyMerger()
            new TakeFirstMerger,
            new TakeLastMerger
        };

        for (auto &strategy : mergeStrategies) {
            // 4. Merge sequencePredictions
            std::vector<short> predictions = strategy->mergeSequencePredictions(sequencePredictions);
            assert(predictions.size() == actual.size());

            // 5. Evaluation
            Evaluation eval(strategy->name(), 2);
            for (int i = 0; i < predictions.size(); i++) {
                eval.prediction(predictions[i], actual[i]);
            }
            std::cout << eval.summaryString() << std::endl;
            // 6. Evaluation with Gap Filler
            int maxGapsize = 10;
            for (int i = 1; i < maxGapsize; i++) {
                Evaluation evalWithGapFiller(strategy->name() + " with gap filler of size " + boost::lexical_cast<std::string>(i), 2);
                // fill the gaps
                predictions = GapFiller::fillGaps(predictions, i);

                for (int i = 0; i < predictions.size(); i++) {
                    eval.prediction(predictions[i], actual[i]);
                }
            }

            delete strategy;
        }
        mergeStrategies.clear();
    }


    // TODO integrate. Fill gaps in sequencePredictions
}

void SoftCutMain::processVideo(Video& video, CaffeClassifier& classifier, std::vector<std::vector<short>>& predictions) {
    std::cout << "Predicting " << video.frames.size() << " frames of video." << std::endl;

    for (int i = 0; i < video.frames.size(); i += sequenceSize + sequenceBatchSize) {
        std::cout << (i * 100) / video.frames.size() << "% " << std::flush;

        // get data for the batch of videos
        SequenceBatch sequenceBatch = getSequenceBatch(video, i);

        // get prediction for frames
        std::vector<short> framePredictions;
        classifier.predict(sequenceBatch.frames, sequenceBatch.labels, resultLayer, dataLayer, framePredictions);
        framePredictions = std::vector<short>(framePredictions.begin(), framePredictions.begin() + sequenceBatch.relevantSize);

        assert(framePredictions.size() == sequenceBatch.relevantSize);
        predictions.push_back(framePredictions);

        framePredictions.clear();
    }
    std::cout << std::endl;
}

void SoftCutMain::writePrediction(std::string videoName,
                                  std::vector<short> actual,
                                  std::vector<short> predictions,
                                  std::vector<std::string> frames) {
    FileWriter writer(outputFile + videoName + ".txt");
    for (int i = 0; i < frames.size(); i++) {
        boost::format line("Frame: %s Predicted: %-3d Actual: %-3d");
        line % frames[i];
        line % predictions[i];
        line % actual[i];
        writer.writeLine(line.str());
    }
    writer.close();
}

SequenceBatch SoftCutMain::getSequenceBatch(Video video, int start) {
    std::vector<cv::Mat> frames;
    std::vector<int> labels;

    for (int i = start; i < start + sequenceBatchSize; i++) {
        for (int j = i; j < i + sequenceSize; j++) {
            // if we are at the end of the video and there are no new
            // frames left to fill the batch, take the last frame multiple times
            int index = std::min(j, static_cast<int>(video.frames.size()) - 1);

            std::string frameFile = video.frames[index];
            cv::Mat frame = cv::imread(frameFile);

            // check if image contains data
            if (!frame.data) {
                std::cerr << "Warning: input image (" << frameFile << ") for caffe classifier is empty." << std::endl;
                exit(1);
            }

            frames.push_back(frame);
            labels.push_back(video.actual[index]);
        }
    }

    SequenceBatch sequenceBatch;
    sequenceBatch.frames = frames;
    sequenceBatch.labels = labels;

    if (start + sequenceBatchSize + sequenceSize - 1 > video.frames.size()) {
        int missingSequences = (int) (start + sequenceBatchSize + sequenceSize - 1 - video.frames.size());
        sequenceBatch.relevantSize = batchSize - (sequenceSize *  missingSequences);
    } else {
        sequenceBatch.relevantSize = batchSize;
    }
    return sequenceBatch;
}

//std::vector<Softcut> mergeDetectedSequences(std::vector<Sequence> sequences, int sequenceSize) {
//    std::vector<int> centers;
//    std::vector<Softcut> softcuts;
//    int i = 0;
//    while (i < sequences.size()) {
//        std::vector<std::string> mergedFrames;
//        // collect sequences of detected sequemces
//        while (sequences[i].clazz) {
//            for (int j = 0; j < sequenceSize; j++) {
//                // check if current frame already is in mergedFrames
//                if (std::find(mergedFrames.begin(), mergedFrames.end(), sequences[i].frames[j]) == mergedFrames.end()) {
//                    // it is not, so add it
//                    mergedFrames.push_back(sequences[i].frames[j]);
//                }
//            }
//            i++;
//        }
//
//        // mergedFrames now contains the full sequence of frames that belong to the softcut (or at least the major part of it)
//        Softcut cut;
//        cut.firstFrame = mergedFrames[0];
//        cut.lastFrame = mergedFrames.back();
//        cut.length = mergedFrames.size();
//        softcuts.push_back(cut);
//
//        i++;
//    }
//
//    return softcuts;
//}

void wrongUsageSoftCut() {
    std::cout << "Usage: sbd --soft_cut" << std::endl;
#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif
    exit(1);
}


