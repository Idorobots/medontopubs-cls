#include "classifier.hpp"

std::map<std::string, size_t> ontomapping(std::string text, std::vector<std::string> terms) {
    std::map<std::string, size_t> mapping;

    std::vector<std::string>::iterator i;
    for(i = terms.begin(); i != terms.end(); ++i) {
        // TODO Actually check for terms, etc.
        mapping[*i] = 23;
    }

    return mapping;
}

std::string summarize(std::string text, std::map<std::string, size_t> terms) {
    // TODO Find best matching sentences.
    return text;
}

ClsResult* classify(std::string text, std::vector<std::string> terms) {
    ClsResult *result = new ClsResult();

    result->terms = ontomapping(text, terms);

    // TODO Cache the best term.
    result->bestTerm = terms[0];
    result->maxScore = result->terms[terms[0]];

    result->summary = summarize(text, result->terms);

    return result;
}
