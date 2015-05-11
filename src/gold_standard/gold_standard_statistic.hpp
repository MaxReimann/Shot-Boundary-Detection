#pragma once

#include "../forwarddeclarations.hpp"
#include "gold_standard_element.hpp"

namespace sbd {

    class GoldStandardStatistic {
    public:
        static void create();
    private:
        static double getMean(std::vector<double> v);
        static void writeResult(std::ofstream& outfile, std::string type, int count, std::vector<double> length);
    };

}
