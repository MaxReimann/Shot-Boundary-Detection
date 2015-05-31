#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd {

    class TransitionGenerator {
    private:
        std::vector<sbd::GoldStandardElement> m_gold;
        std::vector<std::string> m_imagePaths;
        std::string m_dataFolder;
        std::vector<std::string> m_tweenerNames;

        std::string getDatasetName();
    public:
        TransitionGenerator(std::unordered_set<sbd::GoldStandardElement> &gold, std::string dataFolder, std::vector<std::string> imagePaths);
        int createRandomTransition();
        void createRandomTransitions(int amount);
    };

}
