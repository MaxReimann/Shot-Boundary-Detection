#pragma once

#include <caffe/caffe.hpp>
#include <caffe/proto/caffe.pb.h>
#include <caffe/util/io.hpp>
#include <caffe/data_layers.hpp>
#include "../../forwarddeclarations.hpp"

namespace sbd {

class CaffeClassifier {
    public:
        CaffeClassifier(bool cpu,
                        std::string modelFile,
                        std::string protoFile,
                        cv::Size,
                        int channels,
                        bool isDebug);
        virtual ~CaffeClassifier();

        void initializeCaffeNet(bool cpu, std::string modelFile, std::string protoFile);
        void predict(std::vector<cv::Mat> image, std::vector<int> labels, std::string resultLayer, std::string dataLayer, std::vector<float>& predictions);

    protected:
        bool isDebug;
        bool cpu;
        std::string modelFileName;
        std::string protoFileName;
        caffe::Net<float>* caffeNet;
        cv::Size imageSize;
        int channels;
    };

}
