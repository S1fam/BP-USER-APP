#include "input_validator.h"

InputValidationResult
InputValidator::validate(const Automaton& A, const std::vector<std::string>& input)
{
    InputValidationResult res{true, {}};

    for (size_t i = 0; i < input.size(); ++i) {
        if (A.Sigma.find(input[i]) == A.Sigma.end()) {
            res.ok = false;
            res.errors.push_back(
                "Input symbol '" + input[i] +
                "' at position " + std::to_string(i) +
                " is not in Sigma."
            );
        }
    }

    return res;
}