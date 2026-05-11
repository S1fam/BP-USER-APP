/**
 * @file json_loader.cpp
 * @author Jaroslav Ištvan (xistva03)
 * @date 2026
 * @brief Implementace nacteni automatu z JSON souboru
 */

#include "json_loader.h"
#include "pretty_print.h"
#include <fstream>
#include <stdexcept>
#include <json.hpp>
#include <iostream>

Automaton JsonLoader::loadFromFile(const std::string& filename) {
    Automaton automaton;
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
        automaton.type = AutomatonType::HZA;
    }
    else if (type == "CPHZA") {
        automaton.type = AutomatonType::CPHZA;
    }
    else if (type == "VRCPHZA") {
        automaton.type = AutomatonType::VRCPHZA;
    }
    else if (type == "LHZA") {
        automaton.type = AutomatonType::LHZA;
    }
    else {
        throw std::runtime_error("Unknown automaton type");
    }

    automaton.n = j.at("n"); // hloubka automatu

    automaton.Q = j.at("Q").get<std::set<std::string>>(); // mnozina stavu
    automaton.Sigma = j.at("Sigma").get<std::set<std::string>>(); // mnozina vstupnich symbolu
    automaton.Gamma = j.at("Gamma").get<std::set<std::string>>(); // mnozina nevstupnich symbolu

    automaton.s = j.at("s"); // pocatecni stav automatu
    automaton.S = j.at("S"); // pocatecni symbol na zasobniku
    automaton.F = j.at("F").get<std::set<std::string>>(); // mnozina koncovych stavu

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

        // json pro HZA a LHZA ma pole depth, namisto depths
        if (automaton.type == AutomatonType::HZA || automaton.type == AutomatonType::LHZA) {
            r.depths.push_back(rule.at("depth"));
        } else {
            r.depths = rule.at("depths").get<std::vector<size_t>>();
        }

        r.expand_from = rule.at("expand-from").get<std::vector<std::string>>();
        r.expand_to = rule.at("expand-to").get<std::vector<std::string>>();

        // lookahead - oba klice musi byt pritomny zaroven (LHZA)
        if (rule.contains("input-depth") && rule.contains("lookahead")) {
            if (!rule.at("input-depth").is_null() && !rule.at("lookahead").is_null()) {
                r.lookahead_depth = rule.at("input-depth").get<size_t>();
                r.lookahead_symbol = rule.at("lookahead").get<std::string>();
            }
        }

        automaton.rules.push_back(r);
    }

    return automaton;
}