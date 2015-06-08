#include <src/gold_standard/file_reader.hpp>
#include "data_generation.hpp"
#include "transition_generator.hpp"

void wrongUsage();

using namespace sbd;

int DataGenerationMain::main(po::variables_map flagArgs, std::map<std::string, std::string> inputArguments) {

    std::string dataFolder = inputArguments.at("data_folder");
    std::unordered_set<sbd::GoldStandardElement> gold = readGoldStandard(dataFolder);
    TransitionGenerator transitionGenerator(gold, dataFolder, getFileNames(dataFolder));
    transitionGenerator.createRandomTransitions(10000);
    return 0;
}

std::unordered_set<sbd::GoldStandardElement> DataGenerationMain::readGoldStandard(std::string dataFolder) {
    printf("Reading gold standard.\n");

    std::string truthFolder = boost::replace_first_copy(dataFolder, "[type]", "truth");
    std::cout << "Reading truth from " << truthFolder << std::endl;

    FileReader fileReader;
    std::unordered_set<sbd::GoldStandardElement> goldStandard = fileReader.readDir(truthFolder.c_str(), true);

    return goldStandard;
}

std::vector<std::string> DataGenerationMain::getFileNames(std::string dataFolder) {
    printf("Getting frame file names.\n");

    std::vector<boost::filesystem::path> imagePaths;
    std::string extension = ".jpg";
    std::string framesFolder = boost::replace_first_copy(dataFolder, "[type]", "frames");
    std::cout << "Reading frames from " << framesFolder << std::endl;

    boost::filesystem::recursive_directory_iterator rdi(framesFolder);
    boost::filesystem::recursive_directory_iterator end_rdi;
    for (; rdi != end_rdi; rdi++) {
        if (extension.compare(rdi->path().extension().string()) == 0) {
            imagePaths.push_back(rdi->path());
        }
        if (imagePaths.size() % 1000 == 0) {
            std::cout << "imagePaths.size() == " + std::to_string(imagePaths.size()) << std::endl;
        }
    }

    // sort according to int number of frame
    std::sort(imagePaths.begin(), imagePaths.end(), [](boost::filesystem::path aPath, boost::filesystem::path bPath) {
        std::string a = aPath.filename().string();
        std::string b = bPath.filename().string();
        std::string aParent = aPath.parent_path().filename().string();
        std::string bParent = bPath.parent_path().filename().string();

        int parentCompare = aParent.compare(bParent);

        if (parentCompare == 0) {
            return std::stoi(a) < std::stoi(b);
        } else {
            return parentCompare < 0;
        }
    });

    std::vector<std::string> imageStrPaths;
    imageStrPaths.reserve(imagePaths.size());
    for (auto img : imagePaths) {
        imageStrPaths.push_back(img.string());
    }

    return imageStrPaths;
}

void wrongUsageDataGeneration()
{
    std::cout << "Usage: sbd generate <data_folder>" << std::endl;
    std::cout << "  data_folder: Folder for the images and the truth data. Must contain the placeholder [type], which will be replaced by 'frames' or 'truth'" << std::endl;
    std::cout << "               For local execution, just set this to '../resources/[type]/'" << std::endl;
#ifdef _WIN32
    system("pause");
#else
    cv::waitKey(0);
#endif
    exit(1);
}
