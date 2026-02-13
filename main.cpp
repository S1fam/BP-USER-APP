#include "json_loader.h"
#include "input_parser.h"
#include "automaton_validator.h"
#include "input_validator.h"
#include "bfs_simulator.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: automaton <automaton.json> <input>\n";
        return 1;
    }

    Automaton automaton;

    try {
        automaton = JsonLoader::loadFromFile(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "JSON error: " << e.what() << "\n";
        return 1;
    }

    auto result = AutomatonValidator::validate(automaton);
    if (!result.ok) {
        std::cerr << "Invalid automaton definition:\n";
        for (const auto& err : result.errors)
            std::cerr << "  - " << err << "\n";
        return 1;
    }

    auto input = parseInputWord(argv[2]);

    auto inputCheck = InputValidator::validate(automaton, input);
    if (!inputCheck.ok) {
        std::cerr << "Invalid input word:\n";
        for (const auto& e : inputCheck.errors) {
            std::cerr << " - " << e << "\n";
        }
        return 1;
    }

    BFSSimulator sim(automaton, input);
    sim.run();

    return 0;
}