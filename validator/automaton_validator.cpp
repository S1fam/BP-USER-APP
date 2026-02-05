#include "automaton_validator.h"

ValidationResult AutomatonValidator::validate(const Automaton& Automaton) {
    ValidationResult res{true, {}};

    auto error = [&](const std::string& msg) {
        res.ok = false;
        res.errors.push_back(msg);
    };

    // kontrola mnozin
    if (Automaton.Q.find(Automaton.s) == Automaton.Q.end())
        // pokud .find() nenalezne "s" (iterator skonci na pozici za poslednim prvkem)
        // pak .find() == .end() a prvek se v mnozine nenachazi
        error("Initial state s is not in Q.");

    if (Automaton.Gamma.find(Automaton.S) == Automaton.Gamma.end())
        error("Initial stack symbol S is not in Gamma.");

    for (const auto& f : Automaton.F)
        if (Automaton.Q.find(f) == Automaton.Q.end())
            error("Final state " + f + " is not in Q.");

    // kontrola hloubky
    if (Automaton.n == 0)
        error("Stack depth n must be >= 1.");

    // kontrola pravidel
    for (size_t i = 0; i < Automaton.rules.size(); ++i) {
        const auto& rule = Automaton.rules[i];

        if (Automaton.Q.find(rule.from) == Automaton.Q.end()) { // pokud neexistuje vychozi stav
            error("Rule " + std::to_string(i) + ": from-state not in Q.");
        }

        if (Automaton.Q.find(rule.to) == Automaton.Q.end()) { // pokud neexistuje stav do ktereho mame prejit
            error("Rule " + std::to_string(i) + ": to-state not in Q.");
        }

        if (Automaton.type == AutomatonType::HZA) { // kontrola, zda mame pouze jeden index u HZA
            if (rule.depths.size() != 1) {
                error("Rule " + std::to_string(i) + ": HZA rule must have exactly one depth.");
            }
        }

        // kontrola, zda sedi pocet indexu, prepisovanych nevstupnich symbolu a novych retezcu
        if (rule.depths.size() != rule.expand_from.size() || rule.depths.size() != rule.expand_to.size()) {
            error("Rule " + std::to_string(i) + ": depths / expand-from / expand-to size mismatch.");
        }

        // kontrola, zda pravidla neporusuji maximalni hloubku automatu
        for (size_t d : rule.depths) {
            if (d < 1 || d > Automaton.n) {
                error("Rule " + std::to_string(i) + ": depth out of range.");
            }
        }

        // kontrola, zda se nesnazime pouzivat input u HZA/CPHZA
        if (Automaton.type != AutomatonType::VRCPHZA && rule.input.has_value()) {
            error("Rule " + std::to_string(i) + ": input symbol not allowed for this automaton type.");
        }
    }

    return res;
}