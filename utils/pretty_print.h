#pragma once
#include "configuration.h"
#include "automaton.h"
#include <vector>
#include <string>

void printRemainingInput(const std::vector<std::string>& input, size_t pos);

void printStack(const std::vector<std::string>& stack);

void printCFG(const Configuration& cfg, const std::vector<std::string>& input);

void printNextCFG(const Configuration& cfg, const std::vector<std::string>& input);

void printRule(const Rule& r, size_t index, AutomatonType type);

std::string automatonTypeToString(AutomatonType t);