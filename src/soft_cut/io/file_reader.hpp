#pragma once

#include <src/soft_cut/softcut_detection.hpp>
#include "../../forwarddeclarations.hpp"

namespace sbd {

    class FileReader {
        public:
        static std::vector<Sequence> load(std::string dir, int sequenceSize, std::vector<Sequence>& sequences);
        private:
        static std::vector<std::string> splitLine(std::string basic_string);
    };

}
