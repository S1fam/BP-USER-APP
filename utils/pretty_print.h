#pragma once
#include "configuration.h"
#include "automaton.h"
#include <vector>
#include <string>

void printCFG(const Configuration& cfg,
              const std::vector<std::string>& input);

void printNextCFG(const Configuration& cfg,
                  const std::vector<std::string>& input);

void printRuleHZA(const Rule& r, size_t index);

std::string automatonTypeToString(AutomatonType t);

void printSet(const std::string& name, const std::set<std::string>& s);

void printAutomaton(const Automaton& A);