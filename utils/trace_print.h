/**
 * @file trace_print.h
 * @author Jaroslav Ištvan (xistva03)
 * @date 2026
 * @brief Definice funkce pro vypisovani prubehu vypoctu (trace) do konzole
 */

#pragma once
#include "configuration.h"
#include "automaton.h"

void printTrace(const Configuration& cfg, const std::vector<std::string>& input, const Automaton& A);