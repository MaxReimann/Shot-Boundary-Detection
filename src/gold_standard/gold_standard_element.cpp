#include "gold_standard_element.hpp"
#include <boost/filesystem.hpp>
#include "file_reader.hpp"

using namespace sbd;

GoldStandardElement::GoldStandardElement(std::string n, std::string t, std::string path, int s, int e) {
	videoName = n;
	cutType = t;
	startFrame = s;
	endFrame = e;
	truthFilePath = path;

    FileReader fileReader;
    auto imPath = fp(boost::replace_first_copy(truthFilePath, "truth", "frames"));
    std::string name = fileReader.extractName(imPath.string());
    auto imDir = imPath.parent_path().parent_path() / name; // parentpath skips sbref folder

    if (!boost::filesystem::exists(imDir))
    {
        // some directories are uppercase and are not found automatically
        imDir = imPath.parent_path().parent_path() / boost::to_upper_copy(name);
    }
    
    videoFolderPath = fp(imDir);
}
