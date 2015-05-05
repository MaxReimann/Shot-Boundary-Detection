#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <regex>
#include <boost/regex.hpp>
#include "file_reader.h"
#include "gold_standard_element.h"

void FileReader::read(std::string fileName) {
    std::ifstream file(fileName);
    std::string line;

    boost::cmatch typeMatch;
    boost::cmatch startFrameMatch;
    boost::cmatch endFrameMatch;

    boost::regex typeRegex("type=\\.([a-z]+)");
    boost::regex startFrameRegex("preFNum=\\.([a-z]+)");
    boost::regex endFrameRegex("postFNum=\\.([a-z]+)");

    while (std::getline(file, line)) {
        const char* l = line.c_str();

        bool findType       = boost::regex_match(l, typeMatch, typeRegex);
        bool findStartFrame = boost::regex_match(l, startFrameMatch, startFrameRegex);
        bool findEndFrame   = boost::regex_match(l, endFrameMatch, endFrameRegex);

        if (findType && findStartFrame && findEndFrame) {
            std::cout << "Type: " << typeMatch[1] << " Start Frame: " << startFrameMatch[1] << " End Frame: " << endFrameMatch[1] << '\n';
            GoldStandardElement element(typeMatch[1], startFrameMatch[1], endFrameMatch[1]);
        }
    }
}
