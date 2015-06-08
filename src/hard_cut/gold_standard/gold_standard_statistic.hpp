#pragma once

#include "../../forwarddeclarations.hpp"
#include "gold_standard_element.hpp"
#include <unordered_map>


namespace sbd {
    class GoldStandardStatistic {
    public:
        static void create(std::string dataFolder);
        static void extractCuts(std::string dataFolder, std::string ouputFolder, bool hardCutsOnly, float posOverNegRate = 1.0);
    private:
        typedef std::unordered_map<std::string, std::vector<sbd::GoldStandardElement>> GoldElementDict;
        
        static double getMean(std::vector<double> v);
        static void writeResult(std::ofstream& outfile, std::string type, int count, std::vector<double> length);
        static void fillNegatives(GoldElementDict &directories, float posOverNegRate = 1.0);
        static void copyFiles(std::string outputFolder, GoldElementDict &inputFiles,  bool hardCutsOnly);
    };

}
