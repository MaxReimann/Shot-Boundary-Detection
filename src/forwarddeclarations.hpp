#pragma once

// forward declare classes here, so includes can be eliminated from headers.


// STD includes we need everywhere
#include <memory>
#include <iostream>
#include <vector>

// openCV forward declarations
#include <opencv2/opencv.hpp>

namespace cv {
    class Mat;
    class CvSVMParams;
    class CvSVM;
}
