#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <algorithm>
#include <map>
#include <vector>
#include <sstream>
#include <cstring>
#include <string>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
std::string replace(std::string &s, const char* what, const char* toWhat, size_t len);
std::string toLower(std::string &s);
std::map<std::string, size_t> lowerize(const std::map<std::string, size_t> &map);

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

#endif // __UTIL_HPP__
