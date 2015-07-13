#include <iostream>
#include <fstream>
#include "file_writer.hpp"

using namespace std;
using namespace sbd;

FileWriter::FileWriter(std::string fileName) {
    m_file.open (fileName, std::ofstream::trunc);
}

void FileWriter::writeLine(std::string text) {
    m_file << text << endl;
}

void FileWriter::writeLine(float text) {
    std::ostringstream ss;
    ss << text;
    std::string s(ss.str());
    m_file << s << endl;
}

void FileWriter::close() {
    m_file.close();
}
