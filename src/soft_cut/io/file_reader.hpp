#pragma once

#include <src/soft_cut/softcut_detection.hpp>
#include "../../forwarddeclarations.hpp"

namespace sbd {

    class FileReader {
        public:
        static void load(std::string dir, std::vector<Video>& videos);
        private:
        static std::vector<std::string> splitLine(std::string basic_string);
    };

}
