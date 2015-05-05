#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <regex>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include "file_reader.hpp"
#include "gold_standard_element.hpp"

using namespace sbd;

void FileReader::read(std::string fileName, std::vector<GoldStandardElement>& goldStandard) {
    std::ifstream file(fileName);
    std::string line;

    // get the name of the file
    boost::cmatch fileNameMatch;
    boost::regex fileNameRegex(".*?truth.(.*)");
    const char* f = fileName.c_str();
    boost::regex_match(f, fileNameMatch, fileNameRegex);
    const char* name = fileNameMatch[1].first;

    boost::cmatch typeMatch;
    boost::cmatch startFrameMatch;
    boost::cmatch endFrameMatch;

    boost::regex typeRegex(".*?type..([A-Z]+).*");
    boost::regex startFrameRegex(".*?preFNum..([0-9]+).*");
    boost::regex endFrameRegex(".*?postFNum..([0-9]+).*");

    while (std::getline(file, line)) {
        const char* l = line.c_str();

        bool findType       = boost::regex_match(l, typeMatch, typeRegex);
        bool findStartFrame = boost::regex_match(l, startFrameMatch, startFrameRegex);
        bool findEndFrame   = boost::regex_match(l, endFrameMatch, endFrameRegex);

        if (findType && findStartFrame && findEndFrame) {
            const char* type       = typeMatch[1].first;
            const char* startFrame = startFrameMatch[1].first;
            const char* endFrame   = endFrameMatch[1].first;

            GoldStandardElement element(name, type, atoi(startFrame), atoi(endFrame));
            goldStandard.push_back(element);
        }
    }
}

void FileReader::readDir(const char *dir, std::vector<GoldStandardElement>& goldStandard) {
    boost::filesystem::recursive_directory_iterator rdi(dir);
    boost::filesystem::recursive_directory_iterator end_rdi;

    std::string extension = ".xml";

    for (; rdi != end_rdi; rdi++) {
        if (extension.compare((*rdi).path().extension().string()) == 0) {
            read((*rdi).path().string(), goldStandard);
        }
    }
}
