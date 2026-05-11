/**
 * @file bfs_simulator.h
 * @author Jaroslav Ištvan (xistva03)
 * @date 2026
 * @brief Definice tridy pro simulaci automatu pomoci prohledavani do sirky (BFS)
 */

#pragma once
#include "automaton.h"
#include "configuration.h"
#include "cli_args.h"
#include "trace_print.h"
#include <optional>
#include <string>
#include <queue>

class BFSSimulator {
public:
    BFSSimulator(const Automaton& automaton, const std::vector<std::string>& input, 
                 const SimulationOptions& options = {});
    std::optional<Configuration> applyRule(const Configuration& cfg, size_t ruleIndex) const;
    void run();

private:
    const Automaton& A;
    const std::vector<std::string>& input;
    SimulationOptions options;

    Configuration makeStartConfig() const;
    bool tryPopStep(const Configuration& cfg, std::queue<Configuration>& q) const;
    void tryExpandSteps(const Configuration& cfg, std::queue<Configuration>& q) const;
    void printStats(size_t processed, size_t pop_steps, size_t expand_steps) const;
};