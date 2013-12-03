#include "util.hpp"

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

std::map<std::string, size_t> lowerize(const std::map<std::string, size_t> &map) {
    std::map<std::string, size_t> lowerized;
    std::map<std::string, size_t>::const_iterator i;

    for(i = map.begin(); i != map.end(); ++i) {
        std::string str = i->first;
        lowerized[toLower(str)] = i->second;
    }

    return lowerized;
}

