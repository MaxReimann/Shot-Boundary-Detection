#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd
{
    class DataGenerationMain {
    public:
        int main(int argc, char **argv);

    private:
        std::unordered_set<sbd::GoldStandardElement> readGoldStandard(std::string);
        std::vector<std::string> getFileNames(std::string);
    };
}