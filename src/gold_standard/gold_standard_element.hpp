#pragma once

#include "../forwarddeclarations.hpp"

namespace sbd {

    class GoldStandardElement {
    public:
        std::string name;
        std::string type;
        int startFrame;
        int endFrame;
		std::string filePath; //optional
        bool operator==(const GoldStandardElement &other) const {
            return name == other.name && startFrame == other.startFrame && endFrame == other.endFrame;
        }

        GoldStandardElement(std::string, std::string, int, int);
		GoldStandardElement(std::string n, std::string t, std::string path, int s, int e);
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
            ss << gold.name << "-" << gold.startFrame << "-" << gold.endFrame;
            std::string goldString = ss.str();

           return hash<string>()(goldString);
        }
    };
}
