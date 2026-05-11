/**
 * @file main.cpp
 * @author Jaroslav Ištvan (xistva03)
 * @date 2026
 * @brief Vstupni bod programu - zpracovani argumentu, nacteni a validace automatu, 
 * parsovani a validace vstupniho slova, spusteni simulace
 */

#include "cli_args.h"
#include "json_loader.h"
#include "input_parser.h"
#include "automaton_validator.h"
#include "input_validator.h"
#include "bfs_simulator.h"
#include <iostream>

static Automaton loadAutomaton(const std::string& file);
static void validateAutomaton(const Automaton& automaton);
static std::vector<std::string> parseAndValidateInput(const Automaton& automaton, const std::string& rawInput);

int main(int argc, char** argv) {
    auto args = CliArgs::parse(argc, argv);
    auto automaton = loadAutomaton(args.automatonFile);
    validateAutomaton(automaton);
    auto input = parseAndValidateInput(automaton, args.inputWord);

    BFSSimulator sim(automaton, input, args.options);
    sim.run();
}

static Automaton loadAutomaton(const std::string& file) {
    try {
        return JsonLoader::loadFromFile(file);
    } catch (const std::exception& e) {
        std::cerr << "JSON error: " << e.what() << "\n";
        std::exit(1);
    }
}

static void validateAutomaton(const Automaton& automaton) {
    auto result = AutomatonValidator::validate(automaton);
    if (result.ok) return;

    std::cerr << "Invalid automaton definition:\n";
    for (const auto& err : result.errors)
        std::cerr << "  - " << err << "\n";
    std::exit(1);
}

static std::vector<std::string> parseAndValidateInput(const Automaton& automaton, const std::string& rawInput) {
    auto input = parseInputWord(rawInput);

    auto result = InputValidator::validate(automaton, input);
    if (result.ok) return input;

    std::cerr << "Invalid input word:\n";
    for (const auto& err : result.errors)
        std::cerr << "  - " << err << "\n";
    std::exit(1);
}