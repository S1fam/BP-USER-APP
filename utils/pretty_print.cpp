#include "pretty_print.h"
#include <iostream>

static void printRemainingInput(const std::vector<std::string>& input, size_t pos) {
    if (pos >= input.size()) {
        std::cout << "ε";
    } else {
        for (size_t i = pos; i < input.size(); ++i) {
            std::cout << input[i];
        }
    }
}

static void printStack(const std::vector<std::string>& stack) {
    for (const auto& s : stack)
        std::cout << s;
}

void printCFG(const Configuration& cfg, const std::vector<std::string>& input) {
    std::cout << "=== CFG ===\n";
    std::cout << "("
              << cfg.state << ", ";
    printRemainingInput(input, cfg.input_pos);
    std::cout << ", ";
    printStack(cfg.stack);
    std::cout << ")\n";
}

void printNextCFG(const Configuration& cfg, const std::vector<std::string>& input) {
    std::cout << "     next CFG: ("
              << cfg.state << ", ";
    printRemainingInput(input, cfg.input_pos);
    std::cout << ", ";
    printStack(cfg.stack);
    std::cout << ")\n";
}

void printRuleHZA(const Rule& r, size_t index) {
    std::cout << "  [" << index << "] "
              << r.depths[0]
              << r.from
              << r.expand_from[0]
              << " → "
              << r.to
              << r.expand_to[0]
              << "\n";
}

std::string automatonTypeToString(AutomatonType t) {
    switch (t) {
    case AutomatonType::HZA:     return "HZA";
    case AutomatonType::CPHZA:   return "CPHZA";
    case AutomatonType::VRCPHZA: return "VRCPHZA";
    default:                     return "UNKNOWN";
    }
}

void printSet(const std::string& name, const std::set<std::string>& s) {
    std::cout << name << " = { ";
    for (const auto& x : s)
        std::cout << x << " ";
    std::cout << "}\n";
}

void printAutomaton(const Automaton& A) {
    std::cout << "=== AUTOMATON ===\n";
    std::cout << "Type: " << automatonTypeToString(A.type) << "\n";
    std::cout << "n: " << A.n << "\n";
    std::cout << "Start state: " << A.s << "\n";
    std::cout << "Start stack symbol: " << A.S << "\n";

    printSet("Q", A.Q);
    printSet("Sigma", A.Sigma);
    printSet("Gamma", A.Gamma);
    printSet("F", A.F);

    std::cout << "Rules:\n";
    for (size_t i = 0; i < A.rules.size(); ++i) {
        printRuleHZA(A.rules[i], i);
    }

    std::cout << "=================\n";
}