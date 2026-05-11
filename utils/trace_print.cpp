/**
 * @file trace_print.cpp
 * @author Jaroslav Ištvan (xistva03)
 * @date 2026
 * @brief Implementace funkce pro vypisovani prubehu vypoctu (trace) do konzole
 */

#include "trace_print.h"
#include "pretty_print.h"
#include <vector>
#include <iostream>

void printTrace(const Configuration& cfg, const std::vector<std::string>& input, const Automaton& A) {
    std::vector<const Configuration*> trace;

    const Configuration* current = &cfg;
    while (current) { // sbirame cestu zpet
        trace.push_back(current);
        current = current->parent.get();
    }

    std::cout << "===== Prubeh vypoctu =====" << std::endl;

    // tisk od startu
    for (auto it = trace.rbegin(); it != trace.rend(); ++it) {
        const Configuration* c = *it;

        // prvni konfigurace (bez |-e nebo |-p)
        if (c->parent == nullptr) {
            std::cout << "(" << c->state << ", ";
            printRemainingInput(input, c->input_pos);
            std::cout << ", ";
            printStack(c->stack);
            std::cout << ")\n";
            continue;
        }

        // typ kroku
        if (c->applied_rule == SIZE_MAX) {
            std::cout << "|-p ";
        } else {
            std::cout << "|-e ";
        }

        // konfigurace
        std::cout << "(" << c->state << ", ";
        printRemainingInput(input, c->input_pos);
        std::cout << ", ";
        printStack(c->stack);
        std::cout << ")";

        if (c->applied_rule != SIZE_MAX) { // pokud byl krok expanzni
            std::cout << "  ";
            printRule(A.rules[c->applied_rule], c->applied_rule, A.type);
            std::cout << "\n";
        } else {
            std::cout << "\n";
        }
    }
}