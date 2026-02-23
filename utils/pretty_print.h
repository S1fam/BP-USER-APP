#pragma once
#include "configuration.h"
#include "automaton.h"
#include <vector>
#include <string>

void printRemainingInput(const std::vector<std::string>& input, size_t pos);

void printStack(const std::vector<std::string>& stack);

void printStackDetail(const std::vector<std::string>& stack, const Automaton& A);

void printCFG(const Configuration& cfg, const std::vector<std::string>& input, const Automaton& A);

void printNextCFG(const Configuration& cfg, const std::vector<std::string>& input);

void printRule(const Rule& r, size_t index);

std::string automatonTypeToString(AutomatonType t);

void printSet(const std::string& name, const std::set<std::string>& s);

void printAutomaton(const Automaton& A);