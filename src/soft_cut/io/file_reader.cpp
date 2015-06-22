#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "file_reader.hpp"

using namespace sbd;

std::vector<Sequence> FileReader::load(std::string txtFile, int sequenceSize, std::vector<Sequence>& sequences) {
    std::cout << "Reading " << txtFile << " ..." << std::endl;

    std::ifstream input(txtFile);
    std::string line;
    std::vector<std::string> frames;
    std::vector<int> clazzes;
    std::string videoName = "";

    if (input.is_open()) {
        while (std::getline(input, line)) {
            // read line and get frame path and class
            std::vector<std::string> tokens = splitLine(line);
            std::string file = tokens.at(0);
            int clazz = std::stoi(tokens.at(1));
            // get video name
            boost::filesystem::path path(file);
            std::string curVideoName = boost::filesystem::basename(path.parent_path());

            // clear everything when we reach a new video
            if (curVideoName != videoName) {
                frames = std::vector<std::string>();
                clazzes = std::vector<int>();
                videoName = curVideoName;
            }

            // add frame and clazz to vectors
            frames.push_back(file);
            clazzes.push_back(clazz);

            // if sequence is complete, create sequence and
            // add it to the sequence vector
            if (frames.size() >= sequenceSize) {
                // get last 10 frames
                std::vector<std::string> curFrames(frames.end() - sequenceSize, frames.end());
                // if the last 10 frames contain a frame, which does not belong to a soft cut
                // set the current class to 0, otherwise set it to 1
                int curClazz = 0;
                if(std::find(clazzes.end() - sequenceSize, clazzes.end(), 0) != clazzes.end()) {
                    curClazz = 1;
                }

                Sequence seq;
                seq.frames = curFrames;
                seq.clazz = curClazz;
                seq.videoName = videoName;
                sequences.push_back(seq);
            }
        }
        input.close();
    }

    return sequences;
}

std::vector<std::string> FileReader::splitLine(std::string s) {
    std::string delimiter = " ";
    std::vector<std::string> tokens;

    // get frame path
    size_t pos = s.find(delimiter);
    std::string token = s.substr(0, pos);
    tokens.push_back(token);
    // get class value
    token = s.substr(pos + 1, s.size());
    tokens.push_back(token);
    return tokens;
}
