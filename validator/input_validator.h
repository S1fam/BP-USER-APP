#pragma once
#include "automaton.h"
#include <vector>
#include <string>

struct InputValidationResult {
    bool ok;
    std::vector<std::string> errors;
};

class InputValidator {
public:
    static InputValidationResult
    validate(const Automaton& A, const std::vector<std::string>& input);
};