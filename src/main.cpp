#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <src/hard_cut/HardCutMain.hpp>
#include <src/gold_standard/gold_standard_statistic.hpp>
#include <src/data_generation/transition_generator.hpp>
#include <src/soft_cut/SoftCutMain.hpp>
#include <src/data_generation/DataGenerationMain.hpp>
#include "main.hpp"

using namespace sbd;

void wrongUsage();

int main(int argc, char** argv) {
    if (argc < 2) {
        wrongUsage();
    }

    std::string hardCutString = "hard_cut";
    std::string softCutString = "soft_cut";
    std::string generateStr = "generate";
    std::string goldStatisticStr = "gold_statistic";

    if (hardCutString.compare(argv[1]) == 0) {
        HardCutMain hardCutMain;
        hardCutMain.main(argc, argv);

    } else if (softCutString.compare(argv[1]) == 0) {
        SoftCutMain softCutMain;
        softCutMain.main(argc, argv);

    } else if (generateStr.compare(argv[1]) == 0) {
        DataGenerationMain dataGenerationMain;
        dataGenerationMain.main(argc, argv);

    } else if (goldStatisticStr.compare(argv[1]) == 0) {
        std::string dataFolder(argv[2]);
        GoldStandardStatistic::create(dataFolder);

    } else {
        wrongUsage();
    }
}


void wrongUsage() {
    std::cout << "Usage: sbd [hard_cut|soft_cut|generate|gold_statistic] <further_parameter>" << std::endl;
    std::cout << "Decide what you want to execute: "<< std::endl;
    std::cout << "  hard_cut       - detection of hard cuts"<< std::endl;
    std::cout << "  soft_cut       - detection of soft cuts"<< std::endl;
    std::cout << "  generate       - generation of soft cuts"<< std::endl;
    std::cout << "  gold_statistic - calculation of gold statistics"<< std::endl;

#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif
    exit(1);
}