#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>

#include "classifier.hpp"

void present(Classification *result) {
    if(result == NULL) {
        std::cout << "Unable to classify this publication. :(\n";
        return;
    }

    std::vector<Term>::iterator i;

    std::cout << "Best matches:\n";
    for(i = result->terms.begin(); i != result->terms.end(); ++i) {
        std::cout << i->label << " --> " << i->probability << "\n";
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
    if(argc < 2) {
        std::cout << "USAGE: " << argv[0] << " (i|s) PUB.txt\n";
        return 1;
    }

    bool caseSensitive;
    if(strncmp(argv[1], "i", 1) == 0) {
        caseSensitive = false;
    } else if (strncmp(argv[1], "s", 1) == 0) {
        caseSensitive = true;
    } else {
        std::cout << "USAGE: " << argv[0] << " (i|s) PUB.txt\n";
        return 1;
    }

    std::string text = readFile(argv[2]);
    std::map<std::string, size_t> terms;

    std::string term;
    while(std::getline(std::cin, term)) {
        terms[term] = 0;
    }

    classifySeed(time(NULL));
    present(caseSensitive ? classify(text, terms) : iclassify(text, terms));
    return 0;
}
