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
        mapping[*i]++;
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

            if((score != 0) && (text.find(*i) != std::string::npos)) {
                finalMapping[*i] = score;
            }
        }
    }

    return finalMapping;
}

class TermFinder {
  protected:
    std::string term;

  public:
    virtual void set(std::string t) {
        term = t;
    }

    virtual bool operator()(std::string sentence) {
        return (sentence.find(term) != std::string::npos);
    }
};

class ITermFinder : public TermFinder {
  public:
    virtual void set(std::string t) {
        term = t;
        toLower(term);
    }

    virtual bool operator()(std::string sentence) {
        std::string copy = sentence;
        toLower(copy);
        return (copy.find(term) != std::string::npos);
    }
};

std::string summarize(TermFinder &termFinder, const std::string &text, const std::vector<std::pair<std::string, double> > &terms) {
    std::string summary;
    std::vector<std::string> sentences = split(text, '.');

    size_t maxSentences = std::max(5, std::min(20, (int) (0.1 * sentences.size())));
    size_t maxTerms = terms.size() * maxSentences;
    size_t maxChunkSkip = 2 * sentences.size() / maxSentences;
    size_t totalWeight = 0;

    std::vector<std::pair<std::string, double> >::const_iterator i;

    for(i = terms.begin(); i != terms.end(); ++i) {
        totalWeight += i->second;
    }

    double beta = 0.0;
    i = terms.begin();

    std::vector<std::string>::iterator lastSentence = sentences.begin();

    for(size_t numSentences = 0, numTerms = 0;
        numTerms < maxTerms && numSentences < maxSentences && lastSentence != sentences.end();) {
        ++numTerms;

        beta += ((double) (rand() % (2 * totalWeight))) / totalWeight;

        do {
            if(++i == terms.end()) {
                i = terms.begin();
            }

            beta -= ((double) i->second) / totalWeight;
        } while (beta > 0.0);

        termFinder.set(i->first);

        std::vector<std::string>::iterator sentence = lastSentence;
        size_t chunkSkip = maxChunkSkip;

        while(chunkSkip != 0 && sentence != sentences.end()) {
            if(termFinder(*sentence)) break;
            ++sentence;
            // NOTE This guy here makes sure we won't skip too big of a chunk of text an once.
            --chunkSkip;
        }

        if(chunkSkip != 0 && sentence != sentences.end()) {
            summary.append(*sentence);
            summary.append(".");
            ++numSentences;
            lastSentence = ++sentence;
        }
    }

    return summary;
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

std::vector<std::pair<std::string, double> > toPairVector(const std::map<std::string, size_t> &map) {
    std::vector<std::pair<std::string, double> > vec;
    std::map<std::string, size_t>::const_iterator i;
    for(i = map.begin(); i != map.end(); ++i) {
        vec.push_back(std::make_pair(i->first, (double) i->second));
    }

    return vec;
}

ClsResult* classifyImpl(const std::string &text, const std::vector<std::string> &terms) {
    assert(terms.size() != 0);

    std::map<std::string, size_t> mapping = ontoMapping(text, terms);

    if(mapping.size() == 0) {
        return NULL;
    }

    ClsResult *result = new ClsResult();

    result->terms = toPairVector(mapping);
    result->bestTerm = findBest(mapping);
    result->maxScore = mapping[result->bestTerm];
    result->summary = "";

    return result;
}

std::vector<std::string> toVector(const std::map<std::string, size_t> &map) {
    std::vector<std::string> vec;

    for(std::map<std::string, size_t>::const_iterator i = map.begin(); i != map.end(); ++i) {
        vec.push_back(i->first);
    }

    return vec;
}

ClsResult* classify(const std::string &text, const std::map<std::string, size_t> &terms) {
    std::vector<std::string> termsVec = toVector(terms);
    ClsResult *result = classifyImpl(text, termsVec);
    if(result == NULL) {
        return NULL;
    }

    TermFinder tf;
    result->summary = summarize(tf, text, result->terms);

    return result;
}

ClsResult* iclassify(const std::string &text, const std::map<std::string, size_t> &terms) {
    std::string textCopy = text;
    std::vector<std::string> termsCopy = toVector(terms);
    toLower(textCopy);
    std::transform(termsCopy.begin(), termsCopy.end(), termsCopy.begin(), ::toLower);

    ClsResult *result = classifyImpl(textCopy, termsCopy);
    if(result == NULL) {
        return NULL;
    }

    ITermFinder tf;
    result->summary = summarize(tf, text, result->terms);

    return result;
}

void classifySeed(size_t seed) {
    srand(seed);
}
