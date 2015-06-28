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

std::unordered_set<GoldStandardElement> FileReader::readDir(const char *dir, std::string cutType) {
    std::unordered_set<GoldStandardElement> goldStandard;

	if (!boost::filesystem::exists(dir))
	{
		std::cout << "[Filereader::readDir] no such directory: " << dir << std::endl;

#ifdef _WIN32
        system("pause");
#else
        cv::waitKey(0);
#endif
		exit(-1);
	}

    boost::filesystem::recursive_directory_iterator rdi(dir);
    boost::filesystem::recursive_directory_iterator end_rdi;

    std::string extension = ".xml";

    for (; rdi != end_rdi; rdi++) {
        if (extension.compare((*rdi).path().extension().string()) == 0) {
            read((*rdi).path().string(), goldStandard, cutType);
        }
    }

    return goldStandard;
}

void FileReader::read(std::string fileName, std::unordered_set<GoldStandardElement>& goldStandard, std::string cutType) {
    std::string name = extractName(fileName);

    boost::smatch typeMatch;
    boost::smatch startFrameMatch;
    boost::smatch endFrameMatch;

    boost::regex typeRegex;
    if (cutType == "all") {
        typeRegex = boost::regex(".*?type..([A-Z]+).*");
    } else {
        typeRegex = boost::regex(".*?type..(" + cutType + ").*");
    }
    boost::regex startFrameRegex(".*?preFNum..([0-9]+).*");
    boost::regex endFrameRegex  (".*?postFNum..([0-9]+).*");

    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line)) {
//        const char* l = line.c_str();
        bool findType       = boost::regex_match(line, typeMatch, typeRegex);
        bool findStartFrame = boost::regex_match(line, startFrameMatch, startFrameRegex);
        bool findEndFrame   = boost::regex_match(line, endFrameMatch, endFrameRegex);

        if (findType && findStartFrame && findEndFrame) {
            std::string type       = typeMatch[1].str();
            std::string startFrame = startFrameMatch[1].str();
            std::string endFrame   = endFrameMatch[1].str();

            GoldStandardElement element(name, type, fileName, std::stoi(startFrame), std::stoi(endFrame));
            goldStandard.insert(element);
        }
    }
}

std::string FileReader::extractName(std::string fileName) {
    std::string name = boost::filesystem::path(fileName).stem().string();

    std::string prefix("ref_");
    std::size_t pos = name.find(prefix);
    int substrPos = (pos == 0) ? prefix.size() : 0;
    name = name.substr(substrPos);

    return name;
}
