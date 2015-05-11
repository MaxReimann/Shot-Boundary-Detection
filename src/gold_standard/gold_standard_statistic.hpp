#pragma once

#include "../forwarddeclarations.hpp"
#include "gold_standard_element.hpp"

namespace sbd {

    class GoldStandardStatistic {
    public:
        static void create();
    private:
        static double getMean(std::vector<double> v);
    };

}
