/**
 * @file pretty_print.h
 * @author Jaroslav Ištvan (xistva03)
 * @date 2026
 * @brief Definice funkci pro vypisovani do konzole (prezentace konfigurace, pravidel, atd.)
 */

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