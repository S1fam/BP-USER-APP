/**
 * @file automaton.h
 * @author Jaroslav Ištvan (xistva03)
 * @date 2026
 * @brief Definice struktury pro reprezentaci automatu
 */

#pragma once
#include <string>
#include <vector>
#include <set>
#include "rule.h"

enum class AutomatonType {
    HZA,
    CPHZA,
    VRCPHZA,
    LHZA
};

struct Automaton {
    AutomatonType type; // typ automatu

    size_t n; // hloubka zasobniku

    std::set<std::string> Q; // stavy
    std::set<std::string> Sigma; // vstupni symboly
    std::set<std::string> Gamma; // nevstupni symboly

    std::string s; // pocatecni stav
    std::string S; // pocatecni symbolu zasobniku
    std::set<std::string> F; // koncove stavy

    std::vector<Rule> rules; // pravidla
};