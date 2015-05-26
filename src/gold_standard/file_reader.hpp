#pragma once

#include "../forwarddeclarations.hpp"
#include "gold_standard_element.hpp"

namespace sbd {

    class FileReader {
        private:
        public:
            std::string extractName(std::string);
            void read(std::string, std::unordered_set<GoldStandardElement>&, bool cutsOnly);
            std::unordered_set<GoldStandardElement> readDir(const char*, bool cutsOnly);

    };

}
