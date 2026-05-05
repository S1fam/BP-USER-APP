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

void printCFG(const Configuration& cfg, const std::vector<std::string>& input) {
    std::cout << "========== CONFIGURATION ==========\n";
    std::cout << "(" << cfg.state << ", ";
    printRemainingInput(input, cfg.input_pos);
    std::cout << ", ";
    printStack(cfg.stack);
    std::cout << ")\n";
}

void printNextCFG(const Configuration& cfg, const std::vector<std::string>& input) {
    std::cout << " --> next CFG: ("
              << cfg.state << ", ";
    printRemainingInput(input, cfg.input_pos);
    std::cout << ", ";
    printStack(cfg.stack);
    std::cout << ")\n";
}

void printRule(const Rule& r, size_t index, AutomatonType type) {
    std::cout << "[" << (index + 1) << ": ";

    bool is_hza_style = (type == AutomatonType::HZA || type == AutomatonType::LHZA);

    if (is_hza_style) {
        // HZA / LHZA
        std::cout << r.depths[0] << r.from << r.expand_from[0];

        // lookahead u LHZA: hloubka vstupu + vstupni symbol za symbolem zasobniku
        if (r.lookahead_depth.has_value() && r.lookahead_symbol.has_value()) {
            std::cout << r.lookahead_depth.value() << r.lookahead_symbol.value();
        }

        std::cout << " -> " << r.to;
 
        // expand-to: vsechny symboly spojene dohromady
        for (size_t i = 0; i < r.expand_to.size(); ++i) {
            std::cout << r.expand_to[i];
        }
    } else {
        // CPHZA / VRCPHZA
        std::cout << r.from;

        // vstupni symbol u VRCPHZA
        if (r.input.has_value()) {
            std::cout << r.input.value();
        }

        std::cout << "(";
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
    std::cout << "]";
}

std::string automatonTypeToString(AutomatonType t) {
    switch (t) {
    case AutomatonType::HZA:     return "HZA";
    case AutomatonType::CPHZA:   return "CPHZA";
    case AutomatonType::VRCPHZA: return "VRCPHZA";
    case AutomatonType::LHZA:    return "LHZA";
    default:                     return "UNKNOWN";
    }
}