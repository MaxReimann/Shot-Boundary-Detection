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
	std::string dataFolder;
    po::options_description desc("Options");
    desc.add_options()
        ("help, h", "Print help messages")
        ("train", "train and test classifier")
		("generate", "generate softcuts")
		("data_folder", po::value<std::string>(&dataFolder)->required(), 
		"Folder for the images and the truth data. \
		Must contain the placeholder [type], which will be replaced by 'frames' or 'truth'\
		For local execution, just set this to '../resources/[type]/'");


	po::positional_options_description positionalOptions;
	positionalOptions.add("data_folder", 1);


	auto usage = [&positionalOptions,&desc](){
		std::cout << "Shot boundary detection application" << std::endl;
		std::cout << "Usage: sbd [--train|--generate] data_folder" << std::endl;
		rad::OptionPrinter::printStandardAppDesc("sbd-detection",
			std::cout,
			desc,
			&positionalOptions);
	};

    po::variables_map vmap;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vmap); 
        /** --help option
        */
        if (vmap.count("help"))
        {
			usage();
        }

		if (1 != (vmap.count("train") + vmap.count("generate")))
			throw po::error("must specify --train OR --generate");

        po::notify(vmap); // throws on error, so do after help in case 
        // there are any problems 
    }
    catch (po::error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		usage();
    }


    HardCutMain hardCutMain;
    hardCutMain.main(vmap);

}

