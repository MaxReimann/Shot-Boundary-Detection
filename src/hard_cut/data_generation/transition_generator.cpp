#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <regex>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "./transition_generator.hpp"
#include "../gold_standard/gold_standard_element.hpp"
#include <random>
#include <fstream>
#include <map>
#include <functional>
#include <iterator>

using namespace sbd;

// t : current time          i
// b : start value           0
// c : change in value       1
// d : duration              transitionLength
std::map<std::string, std::function<float(float, float, float, float)>> tweenerMap = {
    { "linearTween", [](float t, float b, float c, float d) {
        return c*t / d + b;
    } },

    { "linearTween", [](float t, float b, float c, float d) {
        return c*t/d + b;
    } },

    { "easeInQuad", [](float t, float b, float c, float d) {
        t /= d;
        return c*t*t + b;
    } },

    { "easeOutQuad", [](float t, float b, float c, float d) {
        t /= d;
        return -c * t*(t-2) + b;
    } }
};


TransitionGenerator::TransitionGenerator(std::unordered_set<sbd::GoldStandardElement> &gold,
    std::string dataFolder,
    std::vector<std::string> imagePaths) {

    for (auto el : tweenerMap) {
        m_tweenerNames.push_back(el.first);
    }

    std::vector<sbd::GoldStandardElement> orderedGold(gold.begin(), gold.end());
    std::sort(orderedGold.begin(), orderedGold.end(), [](sbd::GoldStandardElement a, sbd::GoldStandardElement b) {
        return a.startFrame < b.startFrame;
    });

    m_gold = orderedGold;
    m_imagePaths = imagePaths;
    m_dataFolder = dataFolder;

    m_filesTxtOut.open("../output/files.txt");
}

int sbd::TransitionGenerator::createRandomTransition()
{
    enum TransitionType { DISSOLVE, FADE, TYPE_COUNT };
    static const char* transitionNames[TYPE_COUNT] = { "DIS", "FAD" };

    cv::Size size(227, 227);

    std::random_device rd;
    std::mt19937 mt(rd());

    std::uniform_int_distribution<int> transitionTypeDist(0, TYPE_COUNT - 1);
    //TransitionType currentType = static_cast<TransitionType>(transitionTypeDist(mt));
    TransitionType currentType = DISSOLVE;

    std::uniform_int_distribution<int> goldSizeDist(0, static_cast<int>(m_gold.size()) - 2);

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
    //int transitionLength = transitionLengthDist(mt);
    int transitionLength = 10;

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

    std::string frameFolder = boost::replace_first_copy(m_dataFolder, "[type]", "frames");
    std::string truthFolder = boost::replace_first_copy(m_dataFolder, "[type]", "truth");

    std::uniform_int_distribution<int> tweenerDist(0, static_cast<int>(m_tweenerNames.size()) - 1);
    std::string tweenerName = m_tweenerNames[tweenerDist(mt)];
    std::function<float(float, float, float, float)> tweener = tweenerMap[tweenerName];

    std::string datasetName = getDatasetName();
    std::string newDatasetName = datasetName + "-" + transitionNames[currentType] + "-" + tweenerName + "-" + std::to_string(startFrame1) + "-" + std::to_string(startFrame2) + "-" + std::to_string(transitionLength);
    std::string outputFramesFolder = "../output/frames/" + newDatasetName;
    std::string outputTruthFolder = "../output/truth/" + newDatasetName;
    std::string noTransitionFolder = "../output/frames/noTransition-" + datasetName + "-" + std::to_string(startFrame1);

    boost::filesystem::create_directories(outputFramesFolder);
    boost::filesystem::create_directories(noTransitionFolder);

    for (int i = 0; i <= transitionLength; i++) {

        std::string imagePath1 = m_imagePaths[startFrame1 + i];
        std::string imagePath2 = m_imagePaths[startFrame2 + i];

        cv::Mat image1 = cv::imread(imagePath1, CV_LOAD_IMAGE_COLOR);
        cv::Mat image2 = cv::imread(imagePath2, CV_LOAD_IMAGE_COLOR);

        if (image1.data == NULL || image2.data == NULL) {
            std::cout << "image not found: " << imagePath1 << " " << imagePath2 << std::endl;
            return -1;
        }

        cv::Mat result;
	cv::Mat resizedResult;

        float alpha, beta;
        float halfTransitionLength = static_cast<float>(transitionLength / 2);
        if (currentType == DISSOLVE) {
            alpha = tweener(static_cast<float>(i), 0, 1, static_cast<float>(transitionLength));
            beta = 1 - alpha;
        }
        else if (currentType == FADE) {
            if (i <= halfTransitionLength) {
                alpha = 1 - tweener(static_cast<float>(i), 0, 1, halfTransitionLength);
                beta = 0;
            }
            else {
                alpha = 0;
                beta = tweener(i - halfTransitionLength, 0, 1, halfTransitionLength);
            }
        }
        cv::addWeighted(image1, alpha, image2, beta, 0.0, result);
        cv::resize(result, resizedResult, size);
        
        std::string fileName = outputFramesFolder + "/" + std::to_string(i) + ".jpg";
        std::cout << "writing file" << fileName << std::endl;
        cv::imwrite(fileName, resizedResult);
        
        m_filesTxtOut << fileName << " 1" << std::endl;
    }

    for (int i = 0; i <= transitionLength; i++) {

        std::string imagePath1 = m_imagePaths[startFrame1 + i];
        cv::Mat image1 = cv::imread(imagePath1, CV_LOAD_IMAGE_COLOR);

        if (image1.data == NULL) {
            std::cout << "image not found: " << imagePath1 << std::endl;
            return -1;
        }

        
	cv::Mat resizedImage1;
        cv::resize(image1, resizedImage1, size);
        
        std::string fileName = noTransitionFolder + "/" + std::to_string(i) + ".jpg";
        std::cout << "writing file" << startFrame1 << startFrame2 << std::endl;
        cv::imwrite(fileName, resizedImage1);

        m_filesTxtOut << fileName << " 0" << std::endl;
    }

    boost::filesystem::create_directories(outputTruthFolder);
    std::ofstream groundTruthOut;

    // create ground truth xml
    groundTruthOut.open(outputTruthFolder + "/ref_" + newDatasetName + ".xml", std::ios_base::app);
    groundTruthOut << "<!DOCTYPE refSeg SYSTEM \"shotBoundaryReferenceSegmentation.dtd\">" << std::endl;
    groundTruthOut << "<refSeg src = \"" + newDatasetName + ".mpg\" creationMethod = \"MANUAL\" totalFNum = \"" + std::to_string(transitionLength) + "\">" << std::endl;
    groundTruthOut << "<trans type = \"" << transitionNames[currentType] << "\" preFNum = \"0\" postFNum = \"" + std::to_string(transitionLength) + "\" / >" << std::endl;
    groundTruthOut << "< / refSeg>" << std::endl;

    

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

std::string sbd::TransitionGenerator::getDatasetName()
{
    boost::smatch datasetNameMatch;
    boost::regex typeRegex = boost::regex(".*\\[type\\]/([^/]*).*");
    bool findType = boost::regex_match(m_dataFolder, datasetNameMatch, typeRegex);
    if (findType) {
        return datasetNameMatch[1].str();
    } else {
        return "unknown_dataset";
    }
}
