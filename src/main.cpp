#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <src/hard_cut/HardCutMain.hpp>
#include "main.hpp"

using namespace sbd;

int main(int argc, char** argv) {
    // TODO split soft and hard cut

    HardCutMain hardCutMain;
    hardCutMain.main(argc, argv);

}
