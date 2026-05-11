/**
 * @file input_validator.cpp
 * @author Jaroslav Ištvan (xistva03)
 * @date 2026
 * @brief Implementace validace vstupniho slova (kontrola, zda vsechny symboly ve vstupu jsou v mnozine Sigma automatu)
 */

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