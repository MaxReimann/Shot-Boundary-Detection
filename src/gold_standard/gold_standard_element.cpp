#include "gold_standard_element.hpp"

using namespace sbd;

GoldStandardElement::GoldStandardElement(std::string n, std::string t, int s, int e) {
    name = n;
    type = t;
    startFrame = s;
    endFrame = e;
}
