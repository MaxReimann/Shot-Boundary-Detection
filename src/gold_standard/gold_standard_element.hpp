#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd {

    class GoldStandardElement {
    public:
        std::string type;
        int start_frame;
        int end_frame;

        GoldStandardElement(std::string, int, int);
    };

}
