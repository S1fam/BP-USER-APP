#pragma once
#include "automaton.h"
#include <string>

class JsonLoader {
public:
    static Automaton loadFromFile(const std::string& filename);
};