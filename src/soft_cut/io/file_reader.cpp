#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "file_reader.hpp"

using namespace sbd;

void FileReader::load(std::string txtFile, int sequenceSize, std::vector<Video>& videos) {
    std::cout << "Reading " << txtFile << " ..." << std::endl;

    std::ifstream input(txtFile);
    std::string line;
    std::vector<std::string> frames;
    std::vector<int> clazzes;
    std::string videoName = "";

    if (!input.is_open()) {
        throw "Could not read input file";
    }

    std::vector<Sequence> sequences;
    while (std::getline(input, line)) {
        // read line and get frame path and class
        std::vector<std::string> tokens = splitLine(line);
        std::string file = tokens.at(0);
        int clazz = std::stoi(tokens.at(1));

        // skip files, which do not exists
        if (!boost::filesystem::exists(file)) {
            std::cout << "Can't find " << file << std::endl;
            continue;
        }

        // get video name
        boost::filesystem::path path(file);
        std::string curVideoName = boost::filesystem::basename(path.parent_path());

        if (videoName == "") {
            videoName = curVideoName;
        }

        // create new video and clear everything when we reach a new video
        if (curVideoName != videoName) {
            videoName = curVideoName;
            Video video;
            video.videoName = videoName;
            video.sequences = sequences;
            videos.push_back(video);

            std::cout << "Video: " << std::endl;
            std::cout << "    Name: " << videoName << "; Sequences: " << sequences.size() << std::endl;

            sequences = std::vector<Sequence>();
            frames = std::vector<std::string>();
            clazzes = std::vector<int>();
        }

        // add frame and clazz to vectors
        frames.push_back(file);
        clazzes.push_back(clazz);

        // if sequence is complete, create sequence and
        // add it to the sequence vector
        if (frames.size() >= sequenceSize) {
            // get last 10 frames amd classes
            std::vector<std::string> curFrames(frames.end() - sequenceSize, frames.end());
            std::vector<int> curClazzes(clazzes.end() - sequenceSize, clazzes.end());
            // if the last 10 frames contain a frame, which does not belong to a soft cut
            // set the current class to 0, otherwise set it to 1
            int curClazz = 1;
            if (std::find(curClazzes.begin(), curClazzes.end(), 0) != curClazzes.end()) {
                curClazz = 0;
            }

            Sequence seq;
            seq.clazzes = curClazzes;
            seq.frames = curFrames;
            seq.clazz = curClazz;
            sequences.push_back(seq);
        }
    }

    // add the last video
    Video video;
    video.videoName = videoName;
    video.sequences = sequences;
    videos.push_back(video);
    input.close();
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
