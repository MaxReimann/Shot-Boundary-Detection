#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "option_printing/option_printer.hpp"
#include "hard_cut/hardcut_detection.hpp"
#include "gold_standard/gold_standard_statistic.hpp"
#include "data_generation/transition_generator.hpp"
#include "data_generation/data_generation.hpp"
#include "soft_cut/softcut_detection.hpp"

#include "main.hpp"


using namespace sbd;

int main(int argc, char** argv) {
    po::options_description desc("Options");
  
    //specify new commandline flags here
	desc.add_options()
		("hard_cut", "detection of hard cuts")
		("soft_cut", "detection of soft cuts")
		("generate", "generation of soft cuts")
		("gold_statistic", "calculation of gold statistics")
		("extract_cuts", "extraction of hard cuts")
		("help", "print help messages")
        ("no_cache", "disable use of training data cache")
		("classify_folder", po::value<std::string>(), "[opt, for hardcuts] specify external testfolder");

    //specify string arguments here
    std::map<std::string, std::string> inputArguments = { { "data_folder", "" } };
    desc.add_options()
        ("data_folder", po::value<std::string>(&inputArguments["data_folder"]), "folder for the images and the truth data.");


    // declare arguments which are dependent on position
	po::positional_options_description positionalOptions;
	positionalOptions.add("data_folder", 1);
	positionalOptions.add("classify_folder", 2);

    po::variables_map vmap;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc)
            .positional(positionalOptions).run(), vmap); // throws on error 
        // --help option
		if (vmap.count("help"))
		{
			std::map<std::string, std::string> verboseHelp = {
					{ "data_folder", "Folder for the images and the truth data."
					"Must be supplied for every option except soft-cut"
					"Must contain the placeholder [type], which will be replaced by 'frames' or 'truth'"
					"For local execution, just set this to '../resources/[type]/'" },
					{"classify_folder", "Use all of the frames in data_folder"
					"for training and test on classify folder" }
			};
		
            usage(desc, positionalOptions, &verboseHelp);
        }

        if (1 != (vmap.count("hard_cut") + vmap.count("generate") + vmap.count("gold_statistic") + vmap.count("soft_cut")))
			throw po::error("must specify --hard_cut OR --soft_cut OR --gold_statistic OR --generate");

        po::notify(vmap); // throws on error, so do after help in case there are any problems 
    }
    catch (po::error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		usage(desc, positionalOptions);
    }

    
    if (vmap.count("hard_cut")) {
        HardCutMain hardCutMain;
        hardCutMain.main(vmap, inputArguments);
    
    } else if (vmap.count("soft_cut")) {
        SoftCutMain softCutMain;
        softCutMain.main(vmap, inputArguments);

    } else if (vmap.count("generate")) {
        DataGenerationMain dataGenerationMain;
        dataGenerationMain.main(vmap, inputArguments);

    } else if (vmap.count("gold_statistic")) {
        GoldStandardStatistic::create(inputArguments["data_folder"]);
    }
	else if (vmap.count("extract_cuts")) {
		GoldStandardStatistic::extractCuts(inputArguments["data_folder"], "../resources/extracted", true);
	}

#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif
    exit(1);
}



void usage(po::options_description desc,
    po::positional_options_description positionalOptions,
    std::map<std::string, std::string> *verboseExplanation)
{
    std::cout << "Shot boundary detection application" << std::endl;
    std::cout << "Usage: sbd [--hard_cut | --soft_cut | --generate | --gold_statistic] data_folder" << std::endl;
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
