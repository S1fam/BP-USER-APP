#pragma once
#include "configuration.h"
#include "automaton.h"

void printTrace(const Configuration& cfg, const std::vector<std::string>& input, const Automaton& A);