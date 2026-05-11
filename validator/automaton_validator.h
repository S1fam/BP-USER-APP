/**
 * @file automaton_validator.h
 * @author Jaroslav Ištvan (xistva03)
 * @date 2026
 * @brief Definice tridy pro validaci automatu (kontrola spravnosti definice automatu)
 */

#pragma once
#include "automaton.h"
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