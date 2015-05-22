#pragma once

#include "../forwarddeclarations.hpp"
#include "gold_standard_element.hpp"

namespace sbd {

    class FileReader {
        private:
            std::string extractName(std::string);
        public:
            void read(std::string, std::unordered_set<GoldStandardElement>&, bool cutsOnly);
            std::unordered_set<GoldStandardElement> readDir(const char*, bool cutsOnly);

    };

}
