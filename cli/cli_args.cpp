#include "cli_args.h"
#include <stdexcept>

// Pouziti: ./automaton [-v] [-d] [-l <N>] [-h] <automaton.json> <input>
void CliArgs::printUsageAndExit() {
    std::cerr << "Usage: automaton [-v] [-d] [-l <N>] [-h] <automaton.json> <input>\n"
              << "  -v       vypise pocet navstivenych konfiguraci na stderr\n"
              << "  -d       vypise prubehy BFS (debug vystup) na stderr\n"
              << "  -l <N>   maximalni pocet zpracovanych konfiguraci (vychozi: 10000)\n"
              << "  -h       vypise tuto napovedu\n"
              << "  <automaton.json>  cesta k JSON souboru s definici automatu\n"
              << "  <input>  vstupni slovo napr: \"a,b,c\"\n";
              std::exit(1);
}

CliArgs CliArgs::parse(int argc, char** argv) {
    CliArgs args;

    int i = 1;
    while (i < argc) {
        std::string arg = argv[i];
 
        if (arg == "-h" || arg == "--help") {
            printUsageAndExit();
        }
        else if (arg == "-v") {
            args.options.verbose = true;
            i++;
        }
        else if (arg == "-d") {
            args.options.debug = true;
            i++;
        }
        else if (arg == "-l") {
            if (i + 1 >= argc) {
                std::cerr << "Chyba: -l vyzaduje argument.\n";
                printUsageAndExit();
            }
            try {
                int val = std::stoi(argv[i + 1]);
                if (val < 1) throw std::out_of_range("limit musi byt >= 1");
                args.options.limit = static_cast<size_t>(val);
            } catch (...) {
                std::cerr << "Chyba: -l vyzaduje kladne cele cislo.\n";
                printUsageAndExit();
            }
            i += 2;
        }
        else {
            break; // zbytek jsou positional argumenty
        }
    }

    // po prepinacich musi nasledovat prave dva positional argumenty
    if (argc - i != 2) {
        printUsageAndExit();
    }

    args.automatonFile = argv[i];
    args.inputWord     = argv[i + 1];
    return args;
}