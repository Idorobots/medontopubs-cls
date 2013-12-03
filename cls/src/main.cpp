#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>

#include "classifier.hpp"

void present(ClsResult *result) {
    if(result == NULL) {
        std::cout << "Unable to classify this publication. :(\n";
        return;
    }

    std::cout << "Best match:\n";
    std::cout << result->bestTerm << " --> " << result->maxScore << "\n";
    std::cout << "Other matches:\n";

    std::vector<std::pair<std::string, double> >::iterator i;
    for(i = result->terms.begin(); i != result->terms.end(); ++i) {
        std::cout << i->first << " --> " << i->second << "\n";
    }
    std::cout << "Summary: \n" << result->summary << "\n";
}

std::string readFile(std::string fileName) {
    std::ifstream ifs(fileName.c_str());
    std::string text;
    text.assign(std::istreambuf_iterator<char>(ifs),
                std::istreambuf_iterator<char>());

    return text;
}

int main(int argc, char** argv) {
    if(argc < 4) {
        std::cout << "USAGE: " << argv[0] << " (i|s) PUB.txt terms ...\n";
        return 1;
    }

    bool caseSensitive;
    if(strncmp(argv[1], "i", 1) == 0) {
        caseSensitive = false;
    } else if (strncmp(argv[1], "s", 1) == 0) {
        caseSensitive = true;
    } else {
        std::cout << "USAGE: " << argv[0] << " (i|s) PUB.txt terms ...\n";
        return 1;
    }

    std::string text = readFile(argv[2]);
    std::map<std::string, size_t> terms;

    for(size_t i = 3; i < argc; ++i) {
        terms[argv[i]] = 1;
    }

    classifySeed(time(NULL));
    present(caseSensitive ? classify(text, terms) : iclassify(text, terms));
    return 0;
}
