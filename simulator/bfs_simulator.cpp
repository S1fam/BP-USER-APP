#include "bfs_simulator.h"
#include "pretty_print.h"
#include <vector>
#include <queue>
#include <iostream>
#include <unordered_set>

// konstruktor
BFSSimulator::BFSSimulator(const Automaton& automaton, const std::vector<std::string>& input,
                           const SimulationOptions& options)
    : A(automaton), input(input), options(options) {}

// aplikace jednoho pravidla
std::optional<Configuration> BFSSimulator::applyRule(const Configuration& config, size_t ruleIndex) const {
    const Rule& rule = A.rules[ruleIndex];

    // kontrola stavu
    if (config.state != rule.from) {
        return std::nullopt; // pokud nelze pravidlo aplikovat
    }

    // kontrola inputu (VRCPHZA)
    size_t new_input_position = config.input_pos;
    if (rule.input.has_value()) { // pokud pravidlo neocekava epsilon vstup
        if (config.input_pos >= input.size()) {
            return std::nullopt; // pokud je vstup spotrebovany (vstup je epsilon)
        }
        if (input[config.input_pos] != rule.input.value()) {
            return std::nullopt; // pokud vstupni symbol neodpovida vstupu pravidla
        }
    }

    // kontrola lookaheadu (LHZA): input-depth k, lookahead symbol a
    // pravidlo se aplikuje, pokud input[input_pos + k - 1] == a
    // pravidlo bez lookaheadu se aplikuje vzdy (lookahead je volitelny)
    if (rule.lookahead_depth.has_value() && rule.lookahead_symbol.has_value()) {
        size_t k = rule.lookahead_depth.value();
        size_t look_pos = config.input_pos + k - 1; // k=1 znamena aktualni symbol
        if (look_pos >= input.size()) {
            return std::nullopt; // pozice lookaheadu je za koncem vstupu
        }
        if (input[look_pos] != rule.lookahead_symbol.value()) {
            return std::nullopt; // ocekavany symbol neodpovida
        }
    }

    // kontrola hloubek
    for (size_t d : rule.depths) {
        if (d == 0 || d > config.stack.size()) {
            return std::nullopt;
        }
    }

    // kontrola expand-from (vrchol = vlevo)
    for (size_t i = 0; i < rule.depths.size(); ++i) {
        auto idx = findNthNonterminalIndex(config.stack, rule.depths[i], A);
        if (!idx.has_value()) { return std::nullopt; }
        if (config.stack[*idx] != rule.expand_from[i]) { return std::nullopt; }
    }


    // prepis od nejvetsi hloubky
    // spocitame vsechny indexy
    std::vector<size_t> indices;
    for (size_t i = 0; i < rule.depths.size(); ++i) {
        auto idx = findNthNonterminalIndex(config.stack, rule.depths[i], A);
        if (!idx || config.stack[*idx] != rule.expand_from[i]) {
            return std::nullopt;
        }
        indices.push_back(*idx);
    }

    // vytvoreni nove konfigurace
    std::vector<std::string> newStack = config.stack;
    // prepis od nejvetsiho indexu
    for (int i = (int)indices.size() - 1; i >= 0; --i) {
        size_t index = indices[i];
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
    next.step_type = StepType::Expand;
    return next;
}

void BFSSimulator::run() {
    std::queue<Configuration> q;
    std::unordered_set<size_t> visited;
    size_t processed = 0;
    size_t pop_steps = 0;
    size_t expand_steps = 0;

    // pocatecni konfigurace
    Configuration start;
    start.state = A.s;
    start.input_pos = 0;
    start.stack = { A.S, "#" };   // vrchol = A.S
    start.parent = nullptr;
    start.applied_rule = SIZE_MAX;

    q.push(start);

    while (!q.empty()) {
        Configuration cfg = q.front(); // zkoumana konfigurace se vezme z pocatku fronty
        q.pop();

        size_t key = hashConfiguration(cfg);
        if (visited.find(key) != visited.end()) { // pokud key je ve visited
            continue; // pokracujeme na dalsi konfiguraci
        }
        visited.insert(key);

        processed++;
        if (cfg.step_type == StepType::Pop) pop_steps++;
        else if (cfg.step_type == StepType::Expand) expand_steps++;
        if (processed >= options.limit) {
            if (options.verbose) {
                std::cerr << "Configurations visited: " << processed
                          << " (pop: " << pop_steps
                          << ", expand: " << expand_steps << ")\n";
            }
            std::cout << "REJECT (limit reached)\n";
            return;
        }

        if (options.debug) {
            printCFG(cfg, input);
        }

        // ACCEPT
        if ((cfg.input_pos == input.size()) && (A.F.find(cfg.state) != A.F.end())
        && (cfg.stack.size() == 1) && (cfg.stack.front() == "#")) {
            printTrace(cfg, input, A);
            if (options.verbose) {
                std::cerr << "Configurations visited: " << processed
                          << " (pop: " << pop_steps
                          << ", expand: " << expand_steps << ")\n";
            }
            std::cout << "ACCEPT\n";
            return;
        }

        // terminalni krok (pop): ma prednost pred expanzemi.
        // pokud vrchol zasobniku == aktualni vstupni symbol, jedina mozna akce je pop.
        // expanze se v takovem pripade vubec nezkousi.
        bool did_pop = false;
        if (!cfg.stack.empty() && cfg.input_pos < input.size()) {
            const std::string& top = cfg.stack.front();
            if (top != "#" &&
                A.Sigma.find(top) != A.Sigma.end() &&
                top == input[cfg.input_pos]) {

                Configuration next = cfg;
                next.stack.erase(next.stack.begin());
                next.input_pos++;
                next.parent = std::make_shared<Configuration>(cfg);
                next.applied_rule = SIZE_MAX;
                next.step_type = StepType::Pop;

                if (options.debug) {
                    std::cout << "terminal step";
                    printNextCFG(next, input);
                }

                q.push(next);
                did_pop = true;
            }
        }

        // expanzni kroky - pouze pokud nebyl proveden pop
        if (!did_pop) {
            for (size_t i = 0; i < A.rules.size(); ++i) {
                auto next = applyRule(cfg, i);
                if (next.has_value()) {
                    if (options.debug) {
                        printRule(A.rules[i], i, A.type);
                        printNextCFG(*next, input);
                    }
                    q.push(next.value());
                }
            }
        }
    }

    if (options.verbose) {
        std::cerr << "Configurations visited: " << processed
                  << " (pop: " << pop_steps
                  << ", expand: " << expand_steps << ")\n";
    }
    std::cout << "REJECT\n"; // pokud jsme v cyklu neprijali vstup
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

// abychom dvakrat nezkoumali stejnou konfiguraci, budeme si uladat hash konfigurace
size_t hashConfiguration(const Configuration& cfg) {
    size_t h = std::hash<std::string>{}(cfg.state);
    h ^= std::hash<size_t>{}(cfg.input_pos) + 0x9e3779b9 + (h<<6) + (h>>2);
 
    for (const auto& s : cfg.stack) {
        h ^= std::hash<std::string>{}(s) + 0x9e3779b9 + (h<<6) + (h>>2);
    }
    return h;
}