#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <regex>
#include <boost/regex.hpp>
#include "truth_reader.h"
#include "truth.h"

using namespace std;

void TruthReader::read() {
    std::ifstream file("../sbref01/ref_anni005.xml");
    std::string line;

    boost::cmatch type_match;
    boost::cmatch start_frame_match;
    boost::cmatch end_frame_match;

    boost::regex type_regex("type=\\.([a-z]+)");
    boost::regex start_frame_regex("preFNum=\\.([a-z]+)");
    boost::regex end_frame_regex("postFNum=\\.([a-z]+)");

    while (std::getline(file, line)) {
        const char* l = line.c_str();

        bool find_type        = boost::regex_match(l, type_match, type_regex);
        bool find_start_frame = boost::regex_match(l, start_frame_match, start_frame_regex);
        bool find_end_frame   = boost::regex_match(l, end_frame_match, end_frame_regex);

        if (find_type && find_start_frame && find_end_frame) {
            std::cout << "Type: " << type_match[1] << " Start Frame: " << start_frame_match[1] << " End Frame: " << end_frame_match[1] << '\n';
        }
    }
}
