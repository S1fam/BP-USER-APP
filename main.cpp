// Pro CLI parsing argumentu
#include "parser/json_loader.h"
#include "validator/automaton_validator.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: automaton <file.json>\n";
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

    std::cout << "Automaton loaded and validated successfully.\n";

    // tady později:
    // Simulator sim(automaton);
    // sim.run();

    return 0;
}