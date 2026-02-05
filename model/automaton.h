#pragma once
#include <string>
#include <vector>
#include <set>
#include "rule.h"

enum class AutomatonType {
    HZA,
    CPHZA,
    VRCPHZA
};

struct Automaton {
    AutomatonType type;

    size_t n; // hloubka zasobniku

    std::set<std::string> Q;
    std::set<std::string> Sigma;
    std::set<std::string> Gamma;

    std::string s;   // pro ulozeni pocatecniho stav
    std::string S;   // pro ulozeni pocatecniho symbolu zasobniku
    std::set<std::string> F;

    std::vector<Rule> rules;
};