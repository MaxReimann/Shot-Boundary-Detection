#pragma once

// forward declare classes here, so includes can be eliminated from headers.


// STD includes we need everywhere
#include <memory>
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_set>

// openCV forward declarations
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
typedef unsigned int uint;
typedef boost::filesystem::path fp;

namespace cv {
    class Mat;
    class CvSVMParams;
    class CvSVM;
}

namespace sbd {
    class GoldStandardElement;
    class SVMLearner;
}
