#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd
{
    class DataGenerationMain {
    public:
        int main(po::variables_map flagArgs, std::map<std::string, std::string> inputArguments);

    private:
        std::unordered_set<sbd::GoldStandardElement> readGoldStandard(std::string);
        std::vector<std::string> getFileNames(std::string);
    };
}