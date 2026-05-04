#include "json_loader.h"
#include "input_parser.h"
#include "automaton_validator.h"
#include "input_validator.h"
#include "bfs_simulator.h"
#include <iostream>

int main(int argc, char** argv) {
    bool verbose = false;
    bool debug = false;
    std::string automaton_file;
    std::string input_word;

    // parsovani argumentu: volitelne prepinace -v a -d (v libovolnem poradi a kombinaci)
    // pouziti: ./automaton [-v] [-d] <automaton.json> <input>
    int arg_idx = 1;
    while (arg_idx < argc) {
        std::string arg = argv[arg_idx];
        if (arg == "-v") { verbose = true; arg_idx++; }
        else if (arg == "-d") { debug = true;   arg_idx++; }
        else break;
    }
    // po prepinacich musi nasledovat prave dva argumenty: json a vstup
    if (argc - arg_idx != 2) {
        std::cerr << "Usage: automaton [-v] [-d] <automaton.json> <input>\n";
        std::cerr << "  -v   vypise pocet navstivenych konfiguraci na stderr\n";
        std::cerr << "  -d   vypise prubehy BFS (debug vystup) na stderr\n";
        return 1;
    }
    automaton_file = argv[arg_idx];
    input_word     = argv[arg_idx + 1];
 
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
 
    BFSSimulator sim(automaton, input, verbose, debug);
    sim.run();

    return 0;
}