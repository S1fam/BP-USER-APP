#include "trace_print.h"
#include <vector>
#include <iostream>

void printTrace(const Configuration& cfg) {
    std::vector<const Configuration*> trace;

    const Configuration* current = &cfg;
    while (current) {
        trace.push_back(current);
        current = current->parent.get();
    }

    // vypisujeme od startu
    for (auto it = trace.rbegin(); it != trace.rend(); ++it) {
        const Configuration* c = *it;
        std::cout << "(" << c->state << ", ";
        std::cout << c->input_pos << ", ";
        std::cout << "[";
        for (const auto& s : c->stack)
            std::cout << s;
        std::cout << "]";
        std::cout << ")\n";
    }
}