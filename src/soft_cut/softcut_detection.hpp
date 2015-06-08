#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd
{
    class SoftCutMain {
    public:
        int main(po::variables_map flagArgs, std::map<std::string, std::string> inputArguments);
    };
}