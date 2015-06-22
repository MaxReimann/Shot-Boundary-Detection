#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd {

    class GoldStandardElement {
    public:
        std::string videoName;
        std::string cutType;
        fp videoFolderPath;
        int startFrame;
        int endFrame;
		std::string truthFilePath; //optional
        bool operator==(const GoldStandardElement &other) const {
            return videoName == other.videoName && startFrame == other.startFrame && endFrame == other.endFrame;
        }

        GoldStandardElement(std::string name, std::string type, std::string path, int startFrame, int endFrame);
    };
}

namespace std {
    template <> struct hash<sbd::GoldStandardElement> {
        std::size_t operator()(const sbd::GoldStandardElement& gold) const {
            using std::size_t;
            using std::hash;
            using std::string;

            std::stringstream ss;
            // Do not include the type in the hashing!
            ss << gold.videoName << "-" << gold.startFrame << "-" << gold.endFrame;
            std::string goldString = ss.str();

           return hash<string>()(goldString);
        }
    };
}
