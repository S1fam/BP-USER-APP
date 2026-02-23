#include "pretty_print.h"
#include <iostream>

void printRemainingInput(const std::vector<std::string>& input, size_t pos) {
    if (pos >= input.size()) {
        std::cout << "ε";
    } else {
        for (size_t i = pos; i < input.size(); ++i) {
            std::cout << input[i];
        }
    }
}

void printStack(const std::vector<std::string>& stack) {
    for (const auto& s : stack) {
        std::cout << s;
    }
}

void printStackDetail(const std::vector<std::string>& stack, const Automaton& A) {
    std::cout << "Stack: ";
    for (size_t i = 0; i < stack.size(); ++i) {
        const std::string& s = stack[i];
        std::cout << "|" << i << ":" << s;

        if (s == "#") {
            std::cout << "(#)";
        }
        else if (A.Sigma.find(s) != A.Sigma.end()) {
            std::cout << "(T)"; // terminal
        }
        else {
            std::cout << "(N)"; // neterminal
        }
        std::cout << "| ";
    }
    std::cout << "\n";
}

void printCFG(const Configuration& cfg, const std::vector<std::string>& input, const Automaton& A) {
    std::cout << "=== CFG ===\n";
    printStackDetail(cfg.stack, A);
    std::cout << "(" << cfg.state << ", ";
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

void printRule(const Rule& r, size_t index) {
    std::cout << (index + 1) << ": ";

    // tisk vstupniho symbolu u VRCPHZA
    if (r.input.has_value()) {
        std::cout << r.input.value();
    }

    std::cout << r.from << "(";

    for (size_t i = 0; i < r.depths.size(); ++i) {
        if (i > 0) { std::cout << ","; }
        std::cout << r.depths[i] << r.expand_from[i];
    }

    std::cout << ") -> " << r.to << "(";

    for (size_t i = 0; i < r.expand_to.size(); ++i) {
        if (i > 0) { std::cout << ","; }
        std::cout << r.expand_to[i];
    }

    std::cout << ")";
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
        printRule(A.rules[i], i);
    }

    std::cout << "=================\n";
}