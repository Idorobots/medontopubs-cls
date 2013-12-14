#ifndef __CLASSIFIER_HPP__
#define __CLASSIFIER_HPP__

#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include "util.hpp"

struct Term {
    std::string label;
    double probability;

    Term(std::string l, double p) : label(l), probability(p) {
    }
};

struct Classification {
    std::vector<Term> terms;
    std::string summary;
};

// Classify text using terms.
bool classify(Classification &result, const std::string &text, const std::map<std::string, size_t> &terms);

// Classify text using terms disregarding letter case.
bool iclassify(Classification &result, const std::string &text, const std::map<std::string, size_t> &terms);

// Used to seed the rng of the summarizer.
void classifySeed(size_t seed);

#endif //__CLASSIFIER_HPP__
