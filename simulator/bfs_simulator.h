#pragma once
#include "automaton.h"
#include "configuration.h"
#include "trace_print.h"
#include <optional>
#include <string>
#include <functional>

class BFSSimulator {
public:
    BFSSimulator(const Automaton& automaton, const std::vector<std::string>& input, 
                 bool verbose = false, bool debug = false);
    std::optional<Configuration>
    applyRule(const Configuration& cfg, size_t ruleIndex) const;
    void run();

private:
    const Automaton& A;
    const std::vector<std::string>& input;
    bool verbose;
    bool debug;
};

std::optional<size_t> findNthNonterminalIndex(const std::vector<std::string>& stack, size_t depth, const Automaton& A);
size_t hashConfiguration(const Configuration& cfg);