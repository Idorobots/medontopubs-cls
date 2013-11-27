#include "classifier.hpp"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if(item.size() > 0) {
            elems.push_back(item);
        }
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::string replace(std::string &s, const char* what, const char* toWhat, size_t len) {
    for(size_t i = 0; i < len; ++i) {
        std::replace(s.begin(), s.end(), what[i], toWhat[i]);
    }

    return s;
}

std::string toLower(std::string &s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::map<std::string, size_t> wordCount(const std::string &text) {
    std::map<std::string, size_t> mapping;
    std::string copy = text;
    std::vector<std::string> words = split(replace(copy, "\n,;.:", "     ", 5), ' ');
    std::vector<std::string>::iterator i;

    for(i = words.begin(); i != words.end(); ++i) {
        mapping[toLower(*i)]++;
    }

    return mapping;
}

std::map<std::string, size_t> ontoMapping(const std::string &text, const std::vector<std::string> &terms) {
    std::map<std::string, size_t> initialMapping = wordCount(text);
    std::map<std::string, size_t> finalMapping;

    std::vector<std::string>::const_iterator i;
    for(i = terms.begin(); i != terms.end(); ++i) {
        if(initialMapping.find(*i) != initialMapping.end()) {
            finalMapping[*i] = initialMapping[*i];
        } else if(i->find(" ") != std::string::npos) {
            std::string term = *i;
            std::vector<std::string> subterms = split(term, ' ');
            std::vector<std::string>::iterator j;
            size_t score = -1;

            for(j = subterms.begin(); j != subterms.end(); ++j) {
                if(initialMapping.find(*j) == initialMapping.end()) {
                    score = 0;
                    break;
                } else {
                    // NOTE Currently assumes the score of a compound term to be the lowest
                    // NOTE score of the subterms. This is plainly wrong. Deal with it.
                    score = std::min(score, initialMapping[*j]);
                }
            }

            if(score != 0) {
                finalMapping[*i] = score;
            }
        }
    }

    return finalMapping;
}

std::string summarize(const std::string &text, const std::map<std::string, size_t> &terms) {
    // TODO Find best matching sentences.
    return text;
}

std::string findBest(const std::map<std::string, size_t> &terms) {
    std::map<std::string, size_t>::const_iterator i = terms.begin();
    std::string best = i->first;
    size_t score = i->second;

    for(; i != terms.end(); ++i) {
        if(score < i->second) {
            best = i->first;
            score = i->second;
        }
    }

    return best;
}

ClsResult* classify(const std::string &text, const std::vector<std::string> &terms) {
    assert(terms.size() != 0);

    ClsResult *result = new ClsResult();

    result->terms = ontoMapping(text, terms);

    result->bestTerm = findBest(result->terms);
    result->maxScore = result->terms[result->bestTerm];

    result->summary = summarize(text, result->terms);

    return result;
}
