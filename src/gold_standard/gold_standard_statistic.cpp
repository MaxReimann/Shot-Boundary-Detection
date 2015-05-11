#include <boost/algorithm/string.hpp>
#include <fstream>
#include "gold_standard_statistic.hpp"
#include "file_reader.hpp"

using namespace sbd;

void GoldStandardStatistic::create(std::string dataFolder) {
    std::string truthFolder = boost::replace_first_copy(dataFolder, "[type]", "truth");

    // read all truth files
    std::cout << "Read truth files..." << std::endl;
    FileReader fileReader;
    std::vector<sbd::GoldStandardElement> goldStandard = fileReader.readDir(truthFolder.c_str(), false);

    // count different cut types
    std::cout << "Count different cut types..." << std::endl;
    int cut = 0;
    int dis = 0;
    std::vector<double> disLength;
    int foi = 0;
    std::vector<double> foiLength;
    int oth = 0;
    std::vector<double> othLength;

    for (std::vector<GoldStandardElement>::size_type i = 0; i != goldStandard.size(); i++) {
        GoldStandardElement element = goldStandard[i];

        if (element.type == "CUT") {
            cut++;
        } else if (element.type == "DIS") {
            dis++;
            disLength.push_back(element.endFrame - element.startFrame);
        } else if (element.type == "OTH" || element.type == "OHT") {
            oth++;
            othLength.push_back(element.endFrame - element.startFrame);
        } else if (element.type == "FOI") {
            foi++;
            foiLength.push_back(element.endFrame - element.startFrame);
        } else {
            std::cout << element.type << std::endl;
        }
    }

    // write results
    std::cout << "Write results..." << std::endl;
    std::ofstream outfile;
    outfile.open ("../gold_standard_statistics.txt");
    writeResult(outfile, "CUT", cut, std::vector<double>());
    writeResult(outfile, "DIS", dis, disLength);
    writeResult(outfile, "OTH", oth, othLength);
    writeResult(outfile, "FOI", foi, foiLength);
    outfile.close();

    std::cout << "Done!" << std::endl;
}


void GoldStandardStatistic::writeResult(std::ofstream& outfile, std::string type, int count, std::vector<double> length) {
    outfile << "--- " << type << " ---" << std::endl;
    outfile << "Count:  " << count << std::endl;
    outfile << "Mean:   " << getMean(length) << std::endl;
    for (int i = 0; i < length.size(); i++) {
        outfile << length[i] << ", ";
    }
    outfile << std::endl << std::endl;
}

double GoldStandardStatistic::getMean(std::vector<double> v) {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    return sum / v.size();
}

