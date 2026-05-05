#pragma once
#include <string>
#include <iostream>
#include <cstdlib>
 
// Moznosti simulace predavane do BFSSimulatoru
struct SimulationOptions {
    bool verbose = false;   // -v: vypise pocet navstivenych konfiguraci na stderr
    bool debug   = false;   // -d: vypise prubehy BFS na stderr
    size_t limit = 25000;   // -l <N>: maximalni pocet zpracovanych konfiguraci
};
 
// Naparsovane argumenty prikazove radky
struct CliArgs {
    std::string automatonFile;
    std::string inputWord;
    SimulationOptions options;
 
    // Parsovani argc/argv.
    // Pri chybe vypise napovedu na stderr a ukonci program s kodem 1.
    static CliArgs parse(int argc, char** argv);
 
private:
    static void printUsageAndExit();
};
