#ifndef __CLASSIFIER_HPP__
#define __CLASSIFIER_HPP__

#include <cassert>
#include <algorithm>
#include <map>
#include <vector>
#include <sstream>
#include <cstring>
#include <string>
#include <utility>

struct ClsResult {
    std::vector<std::pair<std::string, double> > terms;
    double maxScore;
    std::string bestTerm;
    std::string summary;
};

// Classify text using terms.
ClsResult* classify(const std::string &text, const std::map<std::string, size_t> &terms);

// Classify text using terms disregarding letter case.
ClsResult* iclassify(const std::string &text, const std::map<std::string, size_t> &terms);

// Used to seed the rng of the summarizer.
void classifySeed(size_t seed);

#endif //__CLASSIFIER_HPP__
