#pragma once

#include "../forwarddeclarations.hpp"
#include "gold_standard_element.hpp"

namespace sbd {

    class FileReader {
            private:
              void read(std::string, std::vector<GoldStandardElement>&);
              const char* extractName(std::string);
            public:
              void readDir(const char*, std::vector<GoldStandardElement>&);
    };

}
