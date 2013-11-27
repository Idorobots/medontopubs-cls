#ifndef __CLASSIFIER_HPP__
#define __CLASSIFIER_HPP__

#include <map>
#include <vector>
#include <string>

struct ClsResult {
    std::map<std::string, size_t> terms;
    size_t maxScore;
    std::string bestTerm;
    std::string summary;
};

ClsResult* classify(std::string text, std::vector<std::string> terms);

#endif //__CLASSIFIER_HPP__
