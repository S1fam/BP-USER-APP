#pragma once
#include "model/automaton.h"
#include <string>

class JsonLoader {
public:
    static Automaton loadFromFile(const std::string& filename);
};