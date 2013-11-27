#ifndef __CLASSIFIER_HPP__
#define __CLASSIFIER_HPP__

#include <cassert>
#include <algorithm>
#include <map>
#include <vector>
#include <sstream>
#include <cstring>
#include <string>

struct ClsResult {
    std::map<std::string, size_t> terms;
    size_t maxScore;
    std::string bestTerm;
    std::string summary;
};

// Classify text using terms.
ClsResult* classify(const std::string &text, const std::vector<std::string> &terms);

// Classify text using terms disregarding letter case.
ClsResult* iclassify(const std::string &text, const std::vector<std::string> &terms);

#endif //__CLASSIFIER_HPP__
