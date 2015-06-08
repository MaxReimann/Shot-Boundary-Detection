#pragma once

#include "../forwarddeclarations.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace sbd
{
    typedef struct {
        cv::Mat classes;
        cv::Mat values;
    } Features;

    class HardCutMain {
    public:
		int main(po::variables_map vmap);
    private:
        std::unordered_set<sbd::GoldStandardElement> readGoldStandard(std::string);
        std::vector<std::string> getFileNames(std::string);
        Features buildHistogramDifferences(std::vector<std::string> &imagePaths, std::unordered_set<sbd::GoldStandardElement> &gold);
        cv::Ptr<sbd::SVMLearner> trainSVM(Features &trainSet);
        void evaluate(Features &testSet, sbd::SVMLearner *learner);
        void createRandomTransition(std::vector<sbd::GoldStandardElement> &gold, std::vector<std::string> imagePaths);
    };
}