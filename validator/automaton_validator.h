#pragma once
#include "model/automaton.h"
#include <string>
#include <vector>

struct ValidationResult {
    bool ok;
    std::vector<std::string> errors;
};

class AutomatonValidator {
public:
    static ValidationResult validate(const Automaton& automaton);
};