#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <src/hard_cut/hardcut_detection.hpp>
#include "option_printing/option_printer.hpp"
#include "main.hpp"


using namespace sbd;
namespace po = boost::program_options;

int main(int argc, char** argv) {
    // TODO split soft and hard cut
    po::options_description desc("Options");
  
    //specify new commandline flags here
    desc.add_options()
        ("help", "Print help messages")
        ("train", "train and test classifier")
        ("generate", "generate softcuts");

    //specify string arguments here
    std::map<std::string, std::string> inputArguments = { { "data_folder", "" } };
    desc.add_options()
        ("data_folder", po::value<std::string>(&inputArguments["data_folder"])->required(), "Folder for the images and the truth data.");


    // declare arguments which are dependent on position
	po::positional_options_description positionalOptions;
	positionalOptions.add("data_folder", 1);

    po::variables_map vmap;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc)
            .positional(positionalOptions).run(), vmap); // throws on error 
        /** --help option
        */
        if (vmap.count("help"))
        {
            std::map<std::string, std::string> verboseHelp = {
                    { "data_folder", "Folder for the images and the truth data."
                        "Must contain the placeholder [type], which will be replaced by 'frames' or 'truth'"
                        "For local execution, just set this to '../resources/[type]/'" } };
            usage(desc, positionalOptions, &verboseHelp);
        }

		if (1 != (vmap.count("train") + vmap.count("generate")))
			throw po::error("must specify --train OR --generate");

        po::notify(vmap); // throws on error, so do after help in case 
        // there are any problems 
    }
    catch (po::error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		usage(desc, positionalOptions);
    }


    HardCutMain hardCutMain;
    hardCutMain.main(vmap, inputArguments);

}

void usage(po::options_description desc,
    po::positional_options_description positionalOptions,
    std::map<std::string, std::string> *verboseExplanation)
{
    std::cout << "Shot boundary detection application" << std::endl;
    std::cout << "Usage: sbd [--train|--generate] data_folder" << std::endl;
    rad::OptionPrinter::printStandardAppDesc("sbd",
        std::cout, desc, &positionalOptions, verboseExplanation);
    // wait for key, so we can read the console output
#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif
    exit(1);
}