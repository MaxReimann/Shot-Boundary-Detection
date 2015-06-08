#include "SoftCutMain.hpp"

void wrongUsage();

using namespace sbd;

int SoftCutMain::main(int argc, char** argv) {

}

void wrongUsageSoftCut()
{
    std::cout << "Usage: sbd soft_cut" << std::endl;
#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif
    exit(1);
}
