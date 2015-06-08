#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <src/hard_cut/HardCutMain.hpp>
#include "main.hpp"

using namespace sbd;
namespace po = boost::program_options;

int main(int argc, char** argv) {
    // TODO split soft and hard cut
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("help", "Print help messages")
        ("add", "additional options")
        ("like", "this");
    po::variables_map vmap;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vmap); 
        /** --help option
        */
        if (vmap.count("help"))
        {
            wrongUsage()
            std::cout << "Basic Command Line Parameter App" << std::endl
                << desc << std::endl;
            return SUCCESS;
        }

        po::notify(vm); // throws on error, so do after help in case 
        // there are any problems 
    }
    catch (po::error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return ERROR_IN_COMMAND_LINE;
    }


    HardCutMain hardCutMain;
    hardCutMain.main(argc, argv);

}
