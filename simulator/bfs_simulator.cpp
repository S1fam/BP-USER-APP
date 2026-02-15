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
BFSSimulator::applyRule(const Configuration& config, size_t ruleIndex) const {
    const Rule& rule = A.rules[ruleIndex];

    // kontrola stavu
    if (config.state != rule.from) {
        return std::nullopt; // pokud nelze pravidlo aplikovat
    }

    // kontrola inputu (VRCPHZA)
    size_t new_input_position = config.input_pos;
    if (rule.input.has_value()) {
        if (config.input_pos >= input.size()) {
            return std::nullopt; // pokud je vstup spotrebovany
        }
        if (input[config.input_pos] != rule.input.value()) {
            return std::nullopt; // pokud vrchol zasobniku neodpovida vstupu pravidla
        }
        new_input_position++; // vstup se spotrebuje (delame 2x)
    }

    // kontrola hloubek
    for (size_t d : rule.depths) {
        if (d == 0 || d > config.stack.size()) {
            return std::nullopt;
        }
    }

    // BUG - index je depth, ale pristup na index nezohlednuje terminaly
    // kontrola expand-from (vrchol = vlevo)
    for (size_t i = 0; i < rule.depths.size(); ++i) {
        auto idx = findNthNonterminalIndex(config.stack, rule.depths[i], A);
        if (!idx.has_value()) { return std::nullopt; }
        if (config.stack[*idx] != rule.expand_from[i]) { return std::nullopt; }
    }

    // vytvoreni nove konfigurace
    std::vector<std::string> newStack = config.stack;

    // BUG - podobne hornimu, bereme depth, ale na zasobniku jsou terminaly
    // prepis od nejvetsi hloubky
    for (int i = (int)rule.depths.size() - 1; i >= 0; --i) {
        // spocitame vsechny indexy
        std::vector<size_t> indices;
        for (size_t i = 0; i < rule.depths.size(); ++i) {
            auto idx = findNthNonterminalIndex(newStack, rule.depths[i], A);
            if (!idx.has_value()) {
                return std::nullopt;
            }
            indices.push_back(*idx);
        }

        // prepisujeme od nejvetsiho indexu
        for (int i = (int)rule.depths.size() - 1; i >= 0; --i) {
            size_t index = indices[i];
            newStack.erase(newStack.begin() + index);
            const std::string& w = rule.expand_to[i];
            for (auto it = w.rbegin(); it != w.rend(); ++it) {
                newStack.insert(newStack.begin() + index, std::string(1, *it));
            }
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

void BFSSimulator::run() {
    std::queue<Configuration> q;

    // pocatecni konfigurace
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
            printCFG(cfg, input, A);
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

std::optional<size_t> // hledame index v zasobniku od vrcholu, kde se nachazi neterminal hloubky depth
findNthNonterminalIndex(const std::vector<std::string>& stack, size_t depth, const Automaton& A) {
    size_t count = 0;
    for (size_t i = 0; i < stack.size(); ++i) {
        const std::string& s = stack[i];

        // neterminal je v Gamma (je nevstupni/neterminal), neni v Sigma (vstupni/terminal), neni #
        if (s != "#" && A.Gamma.find(s) != A.Gamma.end() && A.Sigma.find(s) == A.Sigma.end()) {
            count++;
            if (count == depth) {
                return i; // index
            }
        }
    }
    return std::nullopt; // neni tolik neterminalu
}
