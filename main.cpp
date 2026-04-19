#include "json_loader.h"
#include "input_parser.h"
#include "automaton_validator.h"
#include "input_validator.h"
#include "bfs_simulator.h"
#include <iostream>

int main(int argc, char** argv) {
    bool verbose = false;
    std::string automaton_file;
    std::string input_word;

    // parsovani argumentu: volitelny prepinac -v
    // pouziti: automaton [-v] <automaton.json> <input>
    if (argc == 3) {
        automaton_file = argv[1];
        input_word = argv[2];
    } else if (argc == 4 && std::string(argv[1]) == "-v") {
        verbose = true;
        automaton_file = argv[2];
        input_word = argv[3];
    } else {
        std::cerr << "Usage: automaton [-v] <automaton.json> <input>\n";
        std::cerr << "-v print number of visited configurations to stderr\n";
        return 1;
    }

    Automaton automaton;

    try {
        automaton = JsonLoader::loadFromFile(automaton_file);
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

    auto input = parseInputWord(input_word);

    auto inputCheck = InputValidator::validate(automaton, input);
    if (!inputCheck.ok) {
        std::cerr << "Invalid input word:\n";
        for (const auto& e : inputCheck.errors) {
            std::cerr << " - " << e << "\n";
        }
        return 1;
    }

    BFSSimulator sim(automaton, input, verbose);
    sim.run();

    return 0;
}