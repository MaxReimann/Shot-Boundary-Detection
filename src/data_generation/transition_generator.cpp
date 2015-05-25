#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <regex>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include "./transition_generator.hpp"
#include "../gold_standard/gold_standard_element.hpp"
#include <random>

using namespace sbd;

TransitionGenerator::TransitionGenerator(std::unordered_set<sbd::GoldStandardElement> &gold,
    std::vector<std::string> imagePaths) {

    std::vector<sbd::GoldStandardElement> orderedGold(gold.begin(), gold.end());
    std::sort(orderedGold.begin(), orderedGold.end(), [](sbd::GoldStandardElement a, sbd::GoldStandardElement b) {
        return a.startFrame <= b.startFrame;
    });

    m_gold = orderedGold;
    m_imagePaths = imagePaths;
}

int sbd::TransitionGenerator::createRandomTransition()
{

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> goldSizeDist(0, m_gold.size() - 2);

    int cutStart1 = goldSizeDist(mt);
    int cutStart2 = goldSizeDist(mt);

    if (cutStart1 == cutStart2) {
        std::cout << "same start frames. retry" << std::endl;
        return -1;
    }

    int sequence1Start = m_gold[cutStart1].endFrame;
    int sequence1End = m_gold[cutStart1 + 1].startFrame;

    int sequence2Start = m_gold[cutStart2].endFrame;
    int sequence2End = m_gold[cutStart2 + 1].startFrame;

    std::uniform_int_distribution<int> transitionLengthDist(10, 25);
    int transitionLength = transitionLengthDist(mt);

    if ((sequence1End - sequence1Start) < transitionLength ||
        (sequence2End - sequence2Start) < transitionLength) {
        std::cout << "retry due to too short sequence";
        return -1;
    }

    std::uniform_int_distribution<int> startFrame1Dist(
        sequence1Start, sequence1End - transitionLength);
    std::uniform_int_distribution<int> startFrame2Dist(
        sequence2Start, sequence2End - transitionLength);

    int startFrame1 = startFrame1Dist(mt);
    int startFrame2 = startFrame2Dist(mt);

    for (size_t i = 0; i < transitionLength; i++) {

        cv::Mat image1 = cv::imread("../resources/frames/senses111/" + std::to_string(startFrame1 + i) + ".jpg", CV_LOAD_IMAGE_COLOR);
        cv::Mat image2 = cv::imread("../resources/frames/senses111/" + std::to_string(startFrame2 + i) + ".jpg", CV_LOAD_IMAGE_COLOR);

        cv::Mat result;
        float alpha = static_cast<float>(i) / transitionLength;
        float beta = 1 - alpha;
        cv::addWeighted(image1, alpha, image2, beta, 0.0, result);

        std::cout << "writing file" << startFrame1 << startFrame2 << std::endl;
        bool b = cv::imwrite(
            "../output/sequence" + std::to_string(startFrame1) + "-" + std::to_string(startFrame2)
            + std::to_string(i) + ".jpg",
            result);
    }

    return 0;
}

void sbd::TransitionGenerator::createRandomTransitions(int amount)
{
    for (size_t i = 0; i < amount; i++)
    {
        if (this->createRandomTransition() != 0) {
            i--;
        }
    }
}
