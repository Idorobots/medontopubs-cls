#include <iostream>
#include <string>
#include <vector>
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

    std::map<std::string, size_t>::iterator i;
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
    if(argc < 3) {
        std::cout << argv[0] << " PUB.txt terms ...\n";
        return 1;
    }

    std::string text = readFile(argv[1]);
    std::vector<std::string> terms;

    for(size_t i = 2; i < argc; ++i) {
        terms.push_back(argv[i]);
    }

    present(classify(text, terms));
    return 0;
}
