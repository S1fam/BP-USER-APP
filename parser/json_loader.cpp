// nacteni automatu do struktury Automaton z json souboru
#include "json_loader.h"
#include <fstream>
#include <stdexcept>
#include <external/include/json.hpp>

Automaton JsonLoader::loadFromFile(const std::string& filename) {
    Automaton Automaton;
    using json = nlohmann::json;

    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file " + filename);
    }

    json j;
    file >> j;

    // typ automatu
    std::string type = j.at("type");
    if (type == "HZA") {
        Automaton.type = AutomatonType::HZA;
    }
    else if (type == "CPHZA") {
        Automaton.type = AutomatonType::CPHZA;
    }
    else if (type == "VRCPHZA") {
        Automaton.type = AutomatonType::VRCPHZA;
    }
    else {
        throw std::runtime_error("Unknown automaton type");
    }

    Automaton.n = j.at("n"); // hloubka automatu

    Automaton.Q = j.at("Q").get<std::set<std::string>>(); // mnozina stavu
    Automaton.Sigma = j.at("Sigma").get<std::set<std::string>>(); // mnozina vstupnich symbolu
    Automaton.Gamma = j.at("Gamma").get<std::set<std::string>>(); // mnozina nevstupnich symbolu

    Automaton.s = j.at("s"); // pocatecni stav automatu
    Automaton.S = j.at("S"); // pocatecni symbol na zasobniku
    Automaton.F = j.at("F").get<std::set<std::string>>(); // mnozina koncovych stavu

    // pravidla
    for (const auto& rule : j.at("rules")) { // rule - pravidlo z pole pravidel v json souboru
        Rule r; // r - pravidlo stuktury Automaton
        r.from = rule.at("from"); // puvodni stav
        r.to = rule.at("to"); // novy stav

        if (rule.contains("input")) { // vstup (u HZA a CPHZA zakazuje validator)
            if (rule.at("input").is_null()) {
                r.input = std::nullopt;
            }
            else {
                r.input = rule.at("input").get<std::string>();
            }
        }

        // json pro HZA ma pole depth, namisto depths
        if (Automaton.type == AutomatonType::HZA) {
            r.depths.push_back(rule.at("depth"));
        } else {
            r.depths = rule.at("depths").get<std::vector<size_t>>();
        }

        r.expand_from = rule.at("expand-from").get<std::vector<std::string>>();
        r.expand_to = rule.at("expand-to").get<std::vector<std::string>>();

        Automaton.rules.push_back(r);
    }

    return Automaton;
}