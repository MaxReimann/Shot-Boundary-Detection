#include <boost/algorithm/string.hpp>
#include <fstream>
#include <numeric>
#include <random>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#include "gold_standard_statistic.hpp"
#include "file_reader.hpp"
#include <src/util.hpp>


using namespace sbd;

void GoldStandardStatistic::create(std::string dataFolder) {
    std::string truthFolder = boost::replace_first_copy(dataFolder, "[type]", "truth");

    // read all truth files
    std::cout << "Read truth files..." << std::endl;
    FileReader fileReader;
    std::unordered_set<sbd::GoldStandardElement> goldStandard = fileReader.readDir(truthFolder.c_str(), false);

    // count different cut types
    std::cout << "Count different cut types..." << std::endl;
    int cut = 0;
    std::vector<double> cutLength;
    int dis = 0;
    std::vector<double> disLength;
    int foi = 0;
    std::vector<double> foiLength;
    int oth = 0;
    std::vector<double> othLength;

    for (const auto& element : goldStandard) {
        if (element.type == "CUT") {
            cut++;
            cutLength.push_back(element.endFrame - element.startFrame);
        } else if (element.type == "DIS") {
            dis++;
            disLength.push_back(element.endFrame - element.startFrame);
        } else if (element.type == "OTH" || element.type == "OHT") {
            oth++;
            othLength.push_back(element.endFrame - element.startFrame);
        } else if (element.type == "FOI") {
            foi++;
            foiLength.push_back(element.endFrame - element.startFrame);
        } else {
            std::cout << element.type << std::endl;
        }
    }

    // write results
    std::cout << "Write results..." << std::endl;
    std::ofstream outfile;
    outfile.open ("../gold_standard_statistics.txt");
    writeResult(outfile, "CUT", cut, cutLength);
    writeResult(outfile, "DIS", dis, disLength);
    writeResult(outfile, "OTH", oth, othLength);
    writeResult(outfile, "FOI", foi, foiLength);
    outfile.close();

    std::cout << "Done!" << std::endl;
}


void GoldStandardStatistic::writeResult(std::ofstream& outfile, std::string type, int count, std::vector<double> length) {
    outfile << "--- " << type << " ---" << std::endl;
    outfile << "Count : " << count << std::endl;
    outfile << "Mean  : " << getMean(length) << std::endl;
    if (length.size() > 0) {
        outfile << "# of frames: " << std::endl;
        outfile << "c(" << length[0];
        for (int i = 1; i < length.size(); i++) {
            outfile << ", " << length[i];
        }
        outfile << ")" << std::endl;
    }
    outfile << std::endl << std::endl;
}

double GoldStandardStatistic::getMean(std::vector<double> v) {
    if (v.size() == 0) return 0.0;
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    return sum / v.size();
}


/*posOverNegRate = equal amount of positives and negatives copied, 1.0 for equal, 0.0 for only positive*/
void GoldStandardStatistic::extractCuts(std::string dataFolder, std::string outputFolder, 
    bool hardCutsOnly, float posOverNegRate)
{
    typedef boost::filesystem::path fp;
	std::string truthFolder = boost::replace_first_copy(dataFolder, "[type]", "truth");

	// read all truth files
	std::cout << "Read truth files..." << std::endl;
	FileReader fileReader;
	std::unordered_set<sbd::GoldStandardElement> goldStandard = fileReader.readDir(truthFolder.c_str(), true);

    std::unordered_set<std::string> skipDirectories;
    GoldElementDict directories;
    
    for (auto &element : goldStandard)
    {
        auto imPath = fp(boost::replace_first_copy(element.filePath, "truth", "frames"));
        std::string name = fileReader.extractName(imPath.string());
        auto imDir = imPath.parent_path().parent_path() / name; //parentpath skips sbref folder

        if (skipDirectories.find(imDir.string()) != skipDirectories.end())
            continue;


        if (!boost::filesystem::exists(imDir))
        {
            //some directories are uppercase and are not found automatically
            imDir = imPath.parent_path().parent_path() / boost::to_upper_copy(name);
            if (!boost::filesystem::exists(imDir))
            {
                std::cout << "could not find directory: " << imDir.string() << std::endl;
                skipDirectories.insert((imPath.parent_path().parent_path() / name).string());
                continue;
            }
        }

        if (directories.find(imDir.string()) != directories.end())
            directories.at(imDir.string()).push_back(element);
        else
            directories[imDir.string()] = { element };

    }

    fillNegatives(directories, posOverNegRate);
    copyFiles(outputFolder, directories, hardCutsOnly);
}


void GoldStandardStatistic::fillNegatives(GoldElementDict &directories, float posOverNegRate)
{
    typedef boost::filesystem::path fp;
    std::random_device rd;
    std::mt19937 mt(rd());

    std::cout << "Pick negative training examples..." << std::endl;

    for (auto &it : directories)
    {
        auto imDir = fp(it.first);
        auto &positives = it.second;
        std::sort(positives.begin(), positives.end(), [](sbd::GoldStandardElement a, sbd::GoldStandardElement b) {
            return a.startFrame <= b.startFrame;
        });

        //last frame known to exist without counting frames
        int maxFrame = positives.back().startFrame;

        int numNegatives = static_cast<int>(round(positives.size() * posOverNegRate));
        std::vector<sbd::GoldStandardElement> negatives;


        //possible frames to be chosen
        std::vector<int> choices(maxFrame);
        std::iota(std::begin(choices), std::end(choices), 0); // Fill with 0, 1, ...,
        random_selector<> selector{};

        int pickedNegatives=0;
        while (pickedNegatives < numNegatives)
        {

            auto choiceIt = selector(choices.begin(), choices.end());

            int chosenIndex = *choiceIt;

            auto equalIndex = [&chosenIndex](const sbd::GoldStandardElement &element) {
                return element.startFrame == chosenIndex || element.endFrame == chosenIndex + 1; };

            if (find_if(positives.begin(), positives.end(), equalIndex) == positives.end())
            {
                //assuming filenames are always ints and files always jpg
                std::string name = std::to_string(chosenIndex) + ".jpg";
                fp imPath = imDir / name;
                negatives.push_back(GoldStandardElement(imDir.leaf().string(), "NEG", imPath.string(), chosenIndex, chosenIndex + 1));
                pickedNegatives++;
            }

            //remove chosenIndex and chosenIndex+1
            choices.erase(choiceIt, choiceIt + 2); 
        }

        positives.insert(positives.end(), negatives.begin(), negatives.end()); //insert into original vector
    }

}


void GoldStandardStatistic::copyFiles(std::string outputFolder, GoldElementDict &inputFiles, bool hardCutsOnly)
{
    typedef boost::filesystem::path fp;
    FileReader fileReader;

    for (auto const &it : inputFiles)
    {
        auto imDir = fp(it.first);
        auto goldElements = it.second;
        int copycount = 0;
        
        std::string name = fileReader.extractName(goldElements.front().filePath);

        fp outPath(outputFolder);
        outPath = outPath / name;

        if (!boost::filesystem::exists(outPath))
        {
            boost::filesystem::create_directory(outPath);
            std::cout << "created dir " << outPath.string() << std::endl;
        }

        for (auto &element : goldElements)
        {
            try
            {
                // extracts only hardcuts and proportional negative examples
                if (hardCutsOnly) 
                {
                    if (element.type == "CUT" || element.type == "NEG")
                    {
                        auto strFrame = [](int frameNumber){return std::to_string(frameNumber) + ".jpg"; };
                        auto in = imDir / strFrame(element.startFrame);
                        auto out = outPath / strFrame(element.startFrame);
                        copy_file(imDir / strFrame(element.startFrame), outPath / strFrame(element.startFrame));
                        copy_file(imDir / strFrame(element.endFrame), outPath / strFrame(element.endFrame));
                        copycount++;
                    }

                }
                else
                    throw std::logic_error("soft cut extraction not implemented");
            }
            catch (std::logic_error &e)
            {
                std::cout << e.what() << std::endl;
            }
        }

        std::cout << "copied " << std::to_string(copycount) << " images to " << outPath.string() << std::endl;
    }
}

