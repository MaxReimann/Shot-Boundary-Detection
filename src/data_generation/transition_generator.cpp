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

int setNumber = 0;

TransitionGenerator::TransitionGenerator(std::unordered_set<sbd::GoldStandardElement> &gold, std::string dataFolder) {

    for (auto el : tweenerMap) {
        m_tweenerNames.push_back(el.first);
    }

    std::vector<sbd::GoldStandardElement> orderedGold(gold.begin(), gold.end());
    std::sort(orderedGold.begin(), orderedGold.end(), [](sbd::GoldStandardElement a, sbd::GoldStandardElement b) {
        if (a.videoName == b.videoName) {
            return a.startFrame < b.startFrame;
        }
        else {
            return a.videoName < b.videoName;
        }
        
    });

    m_gold = orderedGold;
    m_dataFolder = dataFolder;
}

int sbd::TransitionGenerator::writeFilesTxtForTestData() {
    m_filesTxtOut.close();
    m_filesTxtOut.open("/opt/data_sets/video_sbd_dataset/generated_soft_cuts/files-2.txt");

    int currentFrameIdx = 0;
    
    
    for (int goldIndex = 0; goldIndex < m_gold.size(); goldIndex++) {
        auto gold = m_gold[goldIndex];
        // semantic: startFrame/endFrame belong to transition

        while (currentFrameIdx < gold.startFrame) {
            // output the frames before the cut
            fp framePath = gold.videoFolderPath / (std::to_string(currentFrameIdx) + ".jpg");

            m_filesTxtOut << framePath.string() << " 0" << std::endl;
            currentFrameIdx++;
        }
        while (currentFrameIdx <= gold.endFrame) {
            // output the frames within the cut
            fp framePath = gold.videoFolderPath / (std::to_string(currentFrameIdx) + ".jpg");

            m_filesTxtOut << framePath.string() << " 1" << std::endl;
            currentFrameIdx++;
        }

        int offset = goldIndex + 1 < m_gold.size() ? 1 : 0;
        std::string nextVideoName = m_gold[goldIndex + offset].videoName;
        if (gold.videoName != nextVideoName || goldIndex + 1 == m_gold.size()) {
            // we have reached the last gold element in the current video
            // now we have to output the remaining frames
            while (true) {
                fp framePath = gold.videoFolderPath / (std::to_string(currentFrameIdx) + ".jpg");

                if (!boost::filesystem::exists(framePath)) {
                    std::cout << " framePath " << framePath << " does not exist. exit! " << std::endl;
                    break;
                }

                m_filesTxtOut << framePath.string() << " 0" << std::endl;
                currentFrameIdx++;
            }

            currentFrameIdx = 0;
        }
    }

    return 0;
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
        //std::cout << "same start frames. retry" << std::endl;
        return -1;
    }

    int sequence1Start = m_gold[cutStart1].endFrame;
    int sequence1End = m_gold[cutStart1 + 1].startFrame;

    int sequence2Start = m_gold[cutStart2].endFrame;
    int sequence2End = m_gold[cutStart2 + 1].startFrame;

    std::uniform_int_distribution<int> transitionLengthDist(10, 25);
    //int transitionLength = transitionLengthDist(mt);
    int transitionLength = 21 - 1;

    std::uniform_int_distribution<int> flipperDist(-1, 2);
    int flipperParam1 = flipperDist(mt);
    int flipperParam2 = flipperDist(mt);
    

    if ((sequence1End - sequence1Start) < transitionLength ||
        (sequence2End - sequence2Start) < transitionLength) {
        //std::cout << "retry due to too short sequence";
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
    std::string newDatasetName =
        datasetName + "-" +
        transitionNames[currentType] + "-" +
        tweenerName + "-" +
        std::to_string(startFrame1) + "-" +
        std::to_string(startFrame2) + "-" +
        std::to_string(transitionLength) +
        (flipperParam1 < 2 ? ("-" + std::to_string(flipperParam1)) : "") +
        (flipperParam2 < 2 ? ("-" + std::to_string(flipperParam2)) : "");

    //std::string baseFolder = "../output/";
    std::string baseFolder = "/opt/data_sets/video_sbd_dataset/generated_soft_cuts/gen-21-2007-" + std::to_string(setNumber) + "/";
    std::string outputFramesFolder = baseFolder + "frames/" + newDatasetName;
    std::string outputTruthFolder = baseFolder + "truth/" + newDatasetName;
    std::string noTransitionFolder = baseFolder + "frames/noTransition-" + datasetName + "-" + std::to_string(startFrame1);

    boost::filesystem::create_directories(outputFramesFolder);
    boost::filesystem::create_directories(noTransitionFolder);
    
    for (int i = 0; i <= transitionLength; i++) {

        std::string imagePath1 = (m_gold[cutStart1].videoFolderPath / (std::to_string(startFrame1 + i) + ".jpg")).string();
        std::string imagePath2 = (m_gold[cutStart2].videoFolderPath / (std::to_string(startFrame2 + i) + ".jpg")).string();

        cv::Mat image1 = cv::imread(imagePath1, CV_LOAD_IMAGE_COLOR);
        cv::Mat image2 = cv::imread(imagePath2, CV_LOAD_IMAGE_COLOR);

        if (image1.data == NULL || image2.data == NULL) {
            std::cout << "image not found: " << imagePath1 << " " << imagePath2 << std::endl;
            return -1;
        }

        if (flipperParam1 < 2) {
            cv::flip(image1, image1, flipperParam1);
        }
        if (flipperParam2 < 2) {
            cv::flip(image2, image2, flipperParam2);
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
//       std::cout << "writing file" << fileName << std::endl;
        cv::imwrite(fileName, resizedResult);

        m_filesTxtOut << fileName << " 1" << std::endl;
    }

    for (int i = 0; i <= transitionLength; i++) {
        std::string imagePath1 = (m_gold[cutStart1].videoFolderPath / (std::to_string(startFrame1 + i) + ".jpg")).string();
        cv::Mat image1 = cv::imread(imagePath1, CV_LOAD_IMAGE_COLOR);

        if (image1.data == NULL) {
            std::cout << "image not found: " << imagePath1 << std::endl;
            return -1;
        }


        cv::Mat resizedImage1;
        cv::resize(image1, resizedImage1, size);

        std::string fileName = noTransitionFolder + "/" + std::to_string(i) + ".jpg";
//        std::cout << "writing file" << startFrame1 << startFrame2 << std::endl;
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
    m_filesTxtOut.open("/opt/data_sets/video_sbd_dataset/generated_soft_cuts/21-files-" + std::to_string(setNumber) + ".txt");
    for (size_t i = 0; i < amount; i++)
    {
        std::cout << (i * 100) / amount << "% " << std::flush;
        if (this->createRandomTransition() != 0) {
            i--;
        }
    }
    std::cout << std::endl;
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
