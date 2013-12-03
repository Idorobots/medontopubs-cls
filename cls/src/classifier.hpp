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
Classification* classify(const std::string &text, const std::map<std::string, size_t> &terms);

// Classify text using terms disregarding letter case.
Classification* iclassify(const std::string &text, const std::map<std::string, size_t> &terms);

typedef Classification ClsResult; // FIXME Deprecated.

// Used to seed the rng of the summarizer.
void classifySeed(size_t seed);

#endif //__CLASSIFIER_HPP__
