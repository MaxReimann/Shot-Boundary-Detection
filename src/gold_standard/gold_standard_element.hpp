#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd {

    class GoldStandardElement {
    public:
        std::string name;
        std::string type;
        int startFrame;
        int endFrame;

        GoldStandardElement(std::string, std::string, int, int);
    };

}
