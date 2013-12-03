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

template <typename T>
std::map<std::string, double> normalize(const std::map<std::string, T> &mapping) {
    double totalProb = 0.0;
    typename std::map<std::string, T>::const_iterator i;

    for(i = mapping.begin(); i != mapping.end(); ++i) {
        totalProb += (double) i->second;
    }

    std::map<std::string, double> newMapping;

    for(i = mapping.begin(); i != mapping.end(); ++i) {
        newMapping[i->first] = ((double) i->second) / totalProb;
    }

    return newMapping;
}

std::map<std::string, size_t> saturate(const std::map<std::string, size_t> &mapping) {
    std::map<std::string, size_t>::const_iterator i;
    std::map<std::string, size_t> newMapping;

    for(i = mapping.begin(); i != mapping.end(); ++i) {
        newMapping[i->first] = 1 + i->second;
    }

    return newMapping;
}

std::map<std::string, double> wordCount(const std::string &text, size_t smoothing) {
    std::map<std::string, double> mapping;
    std::string copy = text;
    std::vector<std::string> words = split(replace(copy, "\n,;.:", "     ", 5), ' ');
    std::vector<std::string>::iterator i;
    size_t totalWords = 0;

    for(i = words.begin(); i != words.end(); ++i) {
        totalWords++;
        mapping[*i]++;
    }

    std::map<std::string, double>::iterator j;

    for(j = mapping.begin(); j != mapping.end(); ++j) {
        // NOTE Using Laplace smoothing for fun & profit:
        mapping[j->first] = (j->second + smoothing)/(totalWords + mapping.size()*smoothing);
    }

    // NOTE We don't need to normalize since it'll be done later.
    return mapping;
}

std::map<std::string, double> ontoMapping(const std::string &text, const std::map<std::string, double> &terms) {
    std::map<std::string, double> initialMapping = wordCount(text, 5);
    std::map<std::string, double> finalMapping;

    std::map<std::string, double>::const_iterator i;
    for(i = terms.begin(); i != terms.end(); ++i) {
        std::string term = i->first;
        double termProb = i->second;

        if(initialMapping.find(term) != initialMapping.end()) {
            // NOTE Posterior = prior * likelyhood, without normalization.
            finalMapping[term] = termProb * initialMapping[term];

        } else if(term.find(" ") != std::string::npos) {
            std::vector<std::string> subterms = split(term, ' ');
            std::vector<std::string>::iterator j;
            double score = 9001.0; // FIXME infinity plox?

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

            if((score != 0) && (text.find(term) != std::string::npos)) {
                // NOTE Bayes as before.
                finalMapping[term] = termProb * score;
            }
        }
    }

    // NOTE We normalize it here.
    return normalize(finalMapping);
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
    double totalWeight = 0;

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

        // NOTE Use roulette based selection, so non-optimal terms can appear in the summary.
        beta += ( ((double) (rand() % 200) / 100) * totalWeight) / totalWeight;

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

bool pairCompare(const std::pair<std::string, double> &a, const std::pair<std::string, double> &b) {
    return (a.second > b.second);
}

std::vector<std::pair<std::string, double> > toPairVector(const std::map<std::string, double> &map) {
    std::vector<std::pair<std::string, double> > vec;
    vec.reserve(map.size());

    std::map<std::string, double>::const_iterator i;
    for(i = map.begin(); i != map.end(); ++i) {
        vec.push_back(std::make_pair(i->first, i->second));
    }

    std::sort(vec.begin(), vec.end(), &pairCompare);
    return vec;
}

ClsResult* classifyImpl(const std::string &text, const std::map<std::string, size_t> &terms) {
    assert(terms.size() != 0);

    std::vector<std::pair<std::string, double> > mapping;
    mapping = toPairVector(ontoMapping(text, normalize(saturate(terms))));

    if(mapping.size() == 0) {
        return NULL;
    }

    ClsResult *result = new ClsResult();

    result->terms = mapping;
    result->bestTerm = mapping[0].first;
    result->maxScore = mapping[0].second;
    result->summary = "";

    return result;
}

ClsResult* classify(const std::string &text, const std::map<std::string, size_t> &terms) {
    ClsResult *result = classifyImpl(text, terms);

    if(result == NULL) {
        return NULL;
    }

    TermFinder tf;
    result->summary = summarize(tf, text, result->terms);

    return result;
}

std::map<std::string, size_t> lowerize(const std::map<std::string, size_t> &map) {
    std::map<std::string, size_t> lowerized;
    std::map<std::string, size_t>::const_iterator i;

    for(i = map.begin(); i != map.end(); ++i) {
        std::string str = i->first;
        lowerized[toLower(str)] = i->second;
    }
    return lowerized;

}

ClsResult* iclassify(const std::string &text, const std::map<std::string, size_t> &terms) {
    std::string textCopy = text;
    ClsResult *result = classifyImpl(toLower(textCopy), lowerize(terms));
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
