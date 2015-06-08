#pragma once

#include "../forwarddeclarations.hpp"
#include "hardcut_detection.hpp"
#include <random>
#include <iterator>


namespace sbd {
	  void splitTrainTestSets(Features &input, float trainTestRatio, Features &trainSet, Features &testSet);
	  bool findGold(std::string path1, std::string path2, std::unordered_set<sbd::GoldStandardElement> &golds);
      void writeVisualizationData(std::vector<std::string> &imagePaths, std::vector<float> diffs, cv::Mat& gold);


#pragma warning(push)
#pragma warning(disable:4244) //annoying template warning
      //Reference implementation for doing random number selection from a container.
      template <typename RandomGenerator = std::default_random_engine>
      struct random_selector
      {
          random_selector(RandomGenerator g = RandomGenerator(std::random_device()()))
              : gen(g) {}

          template <typename Iter>
          Iter select(Iter start, Iter end) {
              std::uniform_int_distribution<int> dis(0, std::distance(start, end) - 1);
              std::advance(start, dis(gen));
              return start;
          }

          //convenience function
          template <typename Iter>
          Iter operator()(Iter start, Iter end) {
              return select(start, end);
          }

          //convenience function that works on anything with a sensible begin() and end(), and returns with a ref to the value type
          template <typename Container>
          auto operator()(const Container& c) -> decltype(*begin(c))& {
              return *select(begin(c), end(c));
          }

      private:
          RandomGenerator gen;
      };

#pragma warning(pop) 


}

