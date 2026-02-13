#pragma once
#include "automaton.h"
#include "configuration.h"
#include "trace_print.h"
#include <optional>
#include <string>

class BFSSimulator {
public:
    BFSSimulator(const Automaton& automaton, const std::vector<std::string>& input);
    std::optional<Configuration>
    applyRule(const Configuration& cfg, size_t ruleIndex) const;
    void run();

private:
    const Automaton& A;
    const std::vector<std::string>& input;
};