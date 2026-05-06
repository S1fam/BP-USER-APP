#include "bfs_simulator.h"
#include "pretty_print.h"
#include <queue>
#include <iostream>
#include <unordered_set>

static std::optional<size_t> findNthNonterminalIndex(const std::vector<std::string>& stack, 
                                                     size_t depth, const Automaton& A);
static size_t hashConfiguration(const Configuration& cfg);
static bool checkState(const Configuration& cfg, const Rule& rule);
static bool checkInputSymbol(const Configuration& cfg, const Rule& rule, const std::vector<std::string>& input);
static bool checkLookahead(const Configuration& cfg, const Rule& rule, const std::vector<std::string>& input);
static bool checkDepths(const Configuration& cfg, const Rule& rule);
static std::optional<std::vector<size_t>> computeIndices(const Configuration& cfg, const Rule& rule, 
                                                         const Automaton& A);
static std::vector<std::string> buildNewStack(const std::vector<std::string>& stack, const Rule& rule,
                                              const std::vector<size_t>& indices);

BFSSimulator::BFSSimulator(const Automaton& automaton, const std::vector<std::string>& input,
                           const SimulationOptions& options)
    : A(automaton), input(input), options(options) {}

void BFSSimulator::run() {
    std::queue<Configuration> q;
    std::unordered_set<size_t> visited;
    size_t processed    = 0;
    size_t pop_steps    = 0;
    size_t expand_steps = 0;

    q.push(makeStartConfig());

    while (!q.empty()) {
        Configuration cfg = q.front();
        q.pop();

        size_t key = hashConfiguration(cfg);
        if (!visited.insert(key).second) continue;

        processed++;
        if (cfg.step_type == StepType::Pop) pop_steps++;
        if (cfg.step_type == StepType::Expand) expand_steps++;

        if (processed >= options.limit) {
            printStats(processed, pop_steps, expand_steps);
            std::cout << "REJECT (limit reached)\n";
            return;
        }

        if (options.debug) printCFG(cfg, input);

        // pokud ma konfigurace zpracovany vstup, stav z mnoziny F, a na zasobniku pouze #
        if (cfg.input_pos == input.size() && A.F.count(cfg.state)
            && cfg.stack.size() == 1 && cfg.stack.front() == "#") {
            printTrace(cfg, input, A);
            printStats(processed, pop_steps, expand_steps);
            std::cout << "ACCEPT\n";
            return;
        }

        // pop ma prednost – pokud byl proveden, expanze se preskoci
        if (!tryPopStep(cfg, q)) {
            tryExpandSteps(cfg, q);
        }
    }

    printStats(processed, pop_steps, expand_steps);
    std::cout << "REJECT\n";
}

Configuration BFSSimulator::makeStartConfig() const {
    Configuration start;
    start.state = A.s;
    start.input_pos = 0;
    start.stack = { A.S, "#" };
    start.parent = nullptr;
    start.applied_rule = SIZE_MAX;
    start.step_type = StepType::Start;
    return start;
}

bool BFSSimulator::tryPopStep(const Configuration& cfg, std::queue<Configuration>& q) const {
    if (cfg.stack.empty() || cfg.input_pos >= input.size()) return false;

    const std::string& top = cfg.stack.front();
    bool isMatchingTerminal = top != "#" && A.Sigma.count(top) && top == input[cfg.input_pos];
    if (!isMatchingTerminal) return false; // vrchol zasobniku neni shodny se vstupnim symbolem

    // vytvorime konfiguraci vzniklou popnutim
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

    q.push(std::move(next)); // vlozime novou konfiguraci do fronty
    return true;
}

// Zkusime aplikovat kazde expanzni pravidlo, pokud lze pouzit, zaradime vzniklou konfiguraci do fronty
void BFSSimulator::tryExpandSteps(const Configuration& cfg, std::queue<Configuration>& q) const {
    for (size_t i = 0; i < A.rules.size(); ++i) {
        auto next = applyRule(cfg, i);
        if (!next.has_value()) continue;

        if (options.debug) {
            printRule(A.rules[i], i, A.type);
            printNextCFG(*next, input);
        }

        q.push(std::move(*next));
    }
}

std::optional<Configuration> BFSSimulator::applyRule(const Configuration& cfg, size_t ruleIndex) const {
    const Rule& rule = A.rules[ruleIndex];

    if (!checkState(cfg, rule)) return std::nullopt;
    if (!checkInputSymbol(cfg, rule, input)) return std::nullopt;
    if (!checkLookahead(cfg, rule, input)) return std::nullopt;
    if (!checkDepths(cfg, rule)) return std::nullopt;

    auto indices = computeIndices(cfg, rule, A); // vypocet indexu prepisovanych neterminalu na zasobniku
    if (!indices.has_value()) return std::nullopt;

    // tvorba vznikle konfigurace aplikaci pravidla
    Configuration next;
    next.state = rule.to;
    next.input_pos = cfg.input_pos;
    next.stack = buildNewStack(cfg.stack, rule, *indices);
    next.parent = std::make_shared<Configuration>(cfg);
    next.applied_rule = ruleIndex;
    next.step_type = StepType::Expand;
    return next;
}

void BFSSimulator::printStats(size_t processed, size_t pop_steps, size_t expand_steps) const {
    if (options.verbose) {
        std::cerr << "Configurations visited: " << processed
                  << " (pop: " << pop_steps
                  << ", expand: " << expand_steps << ")\n";
    }
}

// Funkce pro naleznuti indexu na zasobniku, na kterem se nachazi neterminal v hloubce depth
static std::optional<size_t> // hloubka neterminalu neni to same co index, protoze na zasobniku mame i terminaly
findNthNonterminalIndex(const std::vector<std::string>& stack, size_t depth, const Automaton& A) {
    size_t count = 0;
    for (size_t i = 0; i < stack.size(); ++i) {
        const std::string& s = stack[i];
        // neterminal: je v Gamma, neni v Sigma, neni dno zasobniku
        if (s != "#" && A.Gamma.count(s) && !A.Sigma.count(s)) {
            if (++count == depth) return i;
        }
    }
    return std::nullopt;
}

/**
 * @brief Hashovací funkce pro konfiguraci automatu.
 * 
 * Implementace využívá algoritmus kombinování hashů inspirovaný knihovnou Boost 
 * (boost::hash_combine) pro dosažení rovnoměrné distribuce.
 * 
 * Převzato a upraveno z: https://www.boost.org/doc/libs/latest/libs/container_hash/doc/html/hash.html
 * ke zdroji bylo přistoupeno 05.05.2026.
 */
static size_t hashConfiguration(const Configuration& cfg) {
    size_t h = std::hash<std::string>{}(cfg.state);
    h ^= std::hash<size_t>{}(cfg.input_pos) + 0x9e3779b9 + (h << 6) + (h >> 2);
    for (const auto& s : cfg.stack) {
        h ^= std::hash<std::string>{}(s) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    return h;
}

static bool checkState(const Configuration& cfg, const Rule& rule) {
    return cfg.state == rule.from;
}

// Zkontroluje vstupni symbol u VRCPHZA, vraci true pokud pravidlo nema vstupni podminku
// nebo pokud se vstup a ocekavany vstupni symbol pravidla shoduji.
static bool checkInputSymbol(const Configuration& cfg, const Rule& rule, const std::vector<std::string>& input) {
    if (!rule.input.has_value()) return true;
    if (cfg.input_pos >= input.size()) return false;
    return input[cfg.input_pos] == rule.input.value();
}

// Zkontroluje lookahead u LHZA, vraci true pokud pravidlo nema lookahead podminku
// nebo pokud se na pozici urcene pravidlem nachazi ocekavany vstup
static bool checkLookahead(const Configuration& cfg, const Rule& rule, const std::vector<std::string>& input) {
    if (!rule.lookahead_depth.has_value() || !rule.lookahead_symbol.has_value()) return true;
    size_t lookPos = cfg.input_pos + rule.lookahead_depth.value() - 1;
    if (lookPos >= input.size()) return false;
    return input[lookPos] == rule.lookahead_symbol.value();
}

// Overi, ze vsechny hloubky jsou v mezich zasobniku.
static bool checkDepths(const Configuration& cfg, const Rule& rule) {
    for (size_t d : rule.depths) {
        if (d == 0 || d > cfg.stack.size()) return false;
    }
    return true;
}

// Najde indexy vsech prepisovanych neterminalu a overi jejich symboly.
// Vraci vektor indexu, nebo nullopt pri neshodne podmince.
static std::optional<std::vector<size_t>>
computeIndices(const Configuration& cfg, const Rule& rule, const Automaton& A) {
    std::vector<size_t> indices;
    for (size_t i = 0; i < rule.depths.size(); ++i) {
        auto idx = findNthNonterminalIndex(cfg.stack, rule.depths[i], A);
        if (!idx.has_value()) return std::nullopt;
        if (cfg.stack[*idx] != rule.expand_from[i]) return std::nullopt;
        indices.push_back(*idx);
    }
    return indices;
}

// Provede prepis zasobniku dle pravidla. Prepis probiha od nejhlubsiho neterminalu.
static std::vector<std::string>
buildNewStack(const std::vector<std::string>& stack, const Rule& rule, const std::vector<size_t>& indices) {
    std::vector<std::string> newStack = stack;
    for (int i = (int)indices.size() - 1; i >= 0; --i) {
        size_t idx = indices[i];
        newStack.erase(newStack.begin() + idx);
        const std::string& replacement = rule.expand_to[i];
        for (auto it = replacement.rbegin(); it != replacement.rend(); ++it) {
            newStack.insert(newStack.begin() + idx, std::string(1, *it));
        }
    }
    return newStack;
}