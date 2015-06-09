#include "softcut_detection.hpp"
#include <glog/logging.h>
#include <src/soft_cut/classification/caffe_classifier.hpp>

void wrongUsage();

using namespace sbd;

int SoftCutMain::main(po::variables_map flagArgs, std::map<std::string, std::string> inputArguments) {
    // Disable logging (1: log warnings, 3: log nothing)
    FLAGS_minloglevel = 1;

    // Caffee parameters
    std::string preModel = "path_to_model";
    std::string protoFile = "path_to_deploy_prototxt";
    bool cpuSetting = false;
    cv::Size size(227, 227);
    int channels = 3;
    bool isDebug = true;
    std::string resultLayer = "argmax";
    std::string dataLayer = "data";
    int batchSize = 64;
    int nrClasses = 2;


    // 1. Get all frames of an video with their class label
    std::vector<cv::Mat> frames;
    std::vector<int> labels;

    // 2. Initialize classifier
    std::cout << "Initialize classifier ..." << std::endl;
    CaffeClassifier classifier(cpuSetting, preModel, protoFile, size, channels, isDebug);

    // 3. Predict a sequence of frames
    std::vector<float> predictions;
    classifier.predict(frames, labels, resultLayer, dataLayer, predictions);

    // 4. Get the prediction result for each of the frames
    for (int k = 0; k < predictions.size(); k++) {
        int pred   = (int) predictions[k];
        int actual = labels.at(k);
    }

    // 5. Write some algorithm, which test different sequences of the video
    //    and which can than tell where exactly a soft cut takes place.

    return 0;
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