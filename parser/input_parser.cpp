#include "input_parser.h"
#include <sstream>
#include <stdexcept>

std::vector<std::string> parseInputWord(const std::string& raw) {
    std::string s = raw;

    // odstraneni { }
    if (!s.empty() && s.front() == '{') s.erase(0, 1);
    if (!s.empty() && s.back() == '}') s.pop_back();

    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, ',')) { // z celeho vstupu v ss vezmeme kazdy token oddeleny ","
        if (token.empty()) {
            throw std::runtime_error("Empty symbol in input word.");
        }
        result.push_back(token); // token vlozime do pole tokenu
    }

    return result;
}