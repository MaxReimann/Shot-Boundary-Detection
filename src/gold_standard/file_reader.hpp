#pragma once

#include "../forwarddeclarations.hpp"
#include "gold_standard_element.hpp"

namespace sbd {

    class FileReader {
        private:
            void read(std::string, std::unordered_set<GoldStandardElement>&, bool cutsOnly);
            std::string extractName(std::string);
        public:
            std::unordered_set<GoldStandardElement> readDir(const char*, bool cutsOnly);
    };

}
