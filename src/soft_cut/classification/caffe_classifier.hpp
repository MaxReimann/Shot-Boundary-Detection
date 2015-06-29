#pragma once
#include "../../forwarddeclarations.hpp"
#ifndef _WIN32
#include <caffe/caffe.hpp>
#include <caffe/proto/caffe.pb.h>
#include <caffe/util/io.hpp>
#include <caffe/data_layers.hpp>

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
        void predict(std::vector<cv::Mat> image, std::vector<int> labels, std::string resultLayer, std::string dataLayer, std::vector<short>& predictions);

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

#else
namespace sbd {
	//Dummy Class for Windows.Does Nothing!
	class CaffeClassifier {
	public:
		CaffeClassifier(bool cpu,
			std::string modelFile,
			std::string protoFile,
			cv::Size,
			int channels,
			bool isDebug){};
		virtual ~CaffeClassifier(){};

		void initializeCaffeNet(bool cpu, std::string modelFile, std::string protoFile){};
		void predict(std::vector<cv::Mat> image, std::vector<int> labels, std::string resultLayer,
			std::string dataLayer, std::vector<float>& predictions){};
	};

}

#endif