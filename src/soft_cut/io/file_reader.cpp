#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "file_reader.hpp"

using namespace sbd;

void FileReader::load(std::string txtFile, std::vector<Video>& videos) {
    std::cout << "Reading " << txtFile << " ..." << std::endl;

    std::ifstream input(txtFile);
    std::string line;
    std::vector<std::string> frames;
    std::vector<short> actual;
    std::string videoName = "";

    if (!input.is_open()) {
        throw "Could not read input file";
    }

    while (std::getline(input, line)) {
        // read line and get frame path and class
        std::vector<std::string> tokens = splitLine(line);
        std::string file = tokens.at(0);
        int clazz = std::stoi(tokens.at(1));

        // skip files, which do not exists
        if (!boost::filesystem::exists(file)) {
            std::cout << "Can't find " << file << "!" << std::endl;
            continue;
        }
        // skip files, which are empty
        if (!cv::imread(file).data) {
            std::cout << "Image " << file << " is emtpy!" << std::endl;
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
            video.actual = actual;
            video.frames = frames;
            videos.push_back(video);

            frames = std::vector<std::string>();
            actual = std::vector<short>();
        }

        // add frame and clazz to vectors
        frames.push_back(file);
        actual.push_back(clazz);
    }

    // add the last video
    Video video;
    video.videoName = videoName;
    video.actual = actual;
    video.frames = frames;
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
