#include "bfs_simulator.h"
#include "pretty_print.h"
#include <vector>
#include <queue>
#include <iostream>

constexpr bool DEBUG = true;

// konstruktor
BFSSimulator::BFSSimulator(const Automaton& automaton,
                           const std::vector<std::string>& input)
    : A(automaton), input(input) {}

// aplikace jednoho pravidla
std::optional<Configuration>
BFSSimulator::applyRule(const Configuration& config, size_t ruleIndex) const
{
    const Rule& rule = A.rules[ruleIndex];

    // kontrola stavu
    if (config.state != rule.from) {
        return std::nullopt;
    }

    // kontrola inputu (VRCPHZA)
    size_t new_input_position = config.input_pos;
    if (rule.input.has_value()) {
        if (config.input_pos >= input.size()) {
            return std::nullopt;
        }
        if (input[config.input_pos] != rule.input.value()) {
            return std::nullopt;
        }
        new_input_position++; // mozna ne?
    }

    // kontrola hloubek
    for (size_t d : rule.depths) {
        if (d == 0 || d > config.stack.size()) {
            return std::nullopt;
        }
    }

    // kontrola expand-from (vrchol = vlevo)
    for (size_t i = 0; i < rule.depths.size(); ++i) {
        size_t index = rule.depths[i] - 1;
        if (config.stack[index] != rule.expand_from[i]) {
            return std::nullopt;
        }
    }

    // vytvoreni nove konfigurace
    std::vector<std::string> newStack = config.stack;

    // prepis od nejvetsi hloubky
    for (int i = (int)rule.depths.size() - 1; i >= 0; --i) {
        size_t index = rule.depths[i] - 1;

        newStack.erase(newStack.begin() + index);

        const std::string& w = rule.expand_to[i];
        for (auto it = w.rbegin(); it != w.rend(); ++it) {
            newStack.insert(newStack.begin() + index, std::string(1, *it));
        }
    }

    Configuration next;
    next.state = rule.to;
    next.input_pos = new_input_position;
    next.stack = std::move(newStack);
    next.parent = std::make_shared<Configuration>(config);
    next.applied_rule = ruleIndex;
    return next;
}

void BFSSimulator::run()
{
    std::queue<Configuration> q;

    // počáteční konfigurace
    Configuration start;
    start.state = A.s;
    start.input_pos = 0;
    start.stack = { A.S, "#" };   // vrchol = A.S
    start.parent = nullptr;
    start.applied_rule = SIZE_MAX;

    q.push(start);

    while (!q.empty()) {
        Configuration cfg = q.front();
        q.pop();

        if (DEBUG) {
            printCFG(cfg, input);
        }

        // ACCEPT
        if (cfg.input_pos == input.size() &&
            A.F.find(cfg.state) != A.F.end()) {
            std::cout << "ACCEPT\n";
            printTrace(cfg);
            return;
        }

        // TERMINÁLNÍ KROK: pop + input++
        if (!cfg.stack.empty() &&
            cfg.input_pos < input.size()) {

            const std::string& top = cfg.stack.front();

            if (top != "#" &&
                A.Sigma.find(top) != A.Sigma.end() &&
                top == input[cfg.input_pos]) {

                Configuration next = cfg;
                next.stack.erase(next.stack.begin()); // pop zleva
                next.input_pos++;
                next.parent = std::make_shared<Configuration>(cfg);
                next.applied_rule = SIZE_MAX;

                if (DEBUG) {
                    std::cout << "  -> terminal step\n";
                    printNextCFG(next, input);
                }

                q.push(next);
            }
        }

        // EXPANZNÍ KROKY
        for (size_t i = 0; i < A.rules.size(); ++i) {
            auto next = applyRule(cfg, i);
            if (next.has_value()) {
                if (DEBUG) {
                    printRuleHZA(A.rules[i], i);
                    printNextCFG(*next, input);
                }
                q.push(next.value());
            }
        }
    }

    std::cout << "REJECT\n";
}
