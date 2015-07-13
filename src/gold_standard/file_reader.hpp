#pragma once

#include "../forwarddeclarations.hpp"
#include "gold_standard_element.hpp"

namespace sbd {

    class FileReader {
        private:
        public:
            std::string extractName(std::string);
            void read(std::string, std::unordered_set<GoldStandardElement>&, std::string cutType);
            std::unordered_set<GoldStandardElement> readDir(const char*, std::string cutType);

    };

}
