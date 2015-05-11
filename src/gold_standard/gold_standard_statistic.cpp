#include "gold_standard_statistic.hpp"
#include "file_reader.hpp"

using namespace sbd;

void GoldStandardStatistic::create() {
    // read all truth files
    FileReader fileReader;
    std::vector<sbd::GoldStandardElement> goldStandard = fileReader.readDir("../resources/truth/", false);

    // count different cut types
    int cut = 0;
    int dis = 0;
    std::vector<double> disLength;
    int oth = 0;
    std::vector<double> othLength;

    for (std::vector<GoldStandardElement>::size_type i = 0; i != goldStandard.size(); i++) {
        GoldStandardElement element = goldStandard[i];

        if (element.type == "CUT") {
            cut++;
        } else if (element.type == "DIS") {
            dis++;
            disLength.push_back(element.endFrame - element.startFrame);
        } else if (element.type == "OTH") {
            oth++;
            othLength.push_back(element.endFrame - element.startFrame);
        } else {
            std::cout << element.type << std::endl;
        }
    }

    // print results
    std::cout << "-----------------------" << std::endl;
    std::cout << "CUT: " << cut << std::endl;
    std::cout << "DIS: " << dis << " MEAN: " << getMean(disLength) << std::endl;
    std::cout << "OTH: " << oth << " MEAN: " << getMean(othLength) <<  std::endl;
}


double GoldStandardStatistic::getMean(std::vector<double> v) {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    return sum / v.size();
}

