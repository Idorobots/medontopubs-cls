#include "classifier.hpp"

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

bool termCompare(const Term &a, const Term &b) {
    return (a.probability > b.probability);
}

std::vector<Term > toTermVector(const std::map<std::string, double> &map) {
    std::vector<Term> vec;
    vec.reserve(map.size());

    std::map<std::string, double>::const_iterator i;
    for(i = map.begin(); i != map.end(); ++i) {
        vec.push_back(Term(i->first, i->second));
    }

    std::sort(vec.begin(), vec.end(), &termCompare);
    return vec;
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

std::string summarize(TermFinder &termFinder, const std::string &text, const std::vector<Term> &terms) {
    std::string summary;
    std::vector<std::string> sentences = split(text, '.');

    size_t maxSentences = std::max(5, std::min(20, (int) (0.1 * sentences.size())));
    size_t maxTerms = terms.size() * maxSentences;
    size_t maxChunkSkip = 2 * sentences.size() / maxSentences;
    double totalProbability = 0;

    std::vector<Term>::const_iterator i;
    for(i = terms.begin(); i != terms.end(); ++i) {
        totalProbability += i->probability;
    }

    double beta = 0.0;
    i = terms.begin();

    std::vector<std::string>::iterator lastSentence = sentences.begin();

    for(size_t numSentences = 0, numTerms = 0;
        numTerms < maxTerms && numSentences < maxSentences && lastSentence != sentences.end();) {
        ++numTerms;

        // NOTE Use roulette based selection, so non-optimal terms can appear in the summary.
        beta += ((double) (rand() % 200) / 100) * totalProbability;

        do {
            if(++i == terms.end()) {
                i = terms.begin();
            }

            beta -= i->probability;
        } while (beta > 0.0);

        termFinder.set(i->label);

        std::vector<std::string>::iterator sentence = lastSentence;
        size_t chunkSkip = maxChunkSkip;

        while(chunkSkip != 0 && sentence != sentences.end()) {
            if(termFinder(*sentence)) break;
            ++sentence;
            // NOTE This guy here makes sure we won't skip too big of a chunk of text an once.
            --chunkSkip;
        }

        if(sentence != sentences.end()) {
            summary.append(*sentence);
            summary.append(".");
            ++numSentences;
            lastSentence = ++sentence;
        }
    }

    return summary;
}

bool classifyImpl(Classification &result, const std::string &text, const std::map<std::string, size_t> &terms) {
    if(terms.size() == 0) {
        return false;
    }

    std::vector<Term> mapping;
    mapping = toTermVector(ontoMapping(text, normalize(saturate(terms))));

    if(mapping.size() == 0) {
        return false;
    }

    result.terms = mapping;
    return true;
}

bool classify(Classification &result, const std::string &text, const std::map<std::string, size_t> &terms) {
    bool success = classifyImpl(result, text, terms);

    if(!success) {
        return false;
    }

    TermFinder tf;
    result.summary = summarize(tf, text, result.terms);

    return true;
}

bool iclassify(Classification &result, const std::string &text, const std::map<std::string, size_t> &terms) {
    std::string textCopy = text;
    bool success = classifyImpl(result, toLower(textCopy), lowerize(terms));

    if(!success) {
        return false;
    }

    ITermFinder tf;
    result.summary = summarize(tf, text, result.terms);

    return true;
}

void classifySeed(size_t seed) {
    srand(seed);
}
