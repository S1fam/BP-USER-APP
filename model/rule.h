#pragma once
#include <string>
#include <vector>
#include <optional>

struct Rule {
    std::string from; // vychozi stav
    std::string to; // novy stav

    std::optional<std::string> input; // vstup - pouze pro VRCPHZA

    // indexy hloubek prepisu (HZA: size = 1, CPHZA/VRCPHZA: size >= 1)
    std::vector<size_t> depths;

    std::vector<std::string> expand_from; // vychozi nevstupni symbol
    std::vector<std::string> expand_to; // novy retezec po prepisu

    std::optional<size_t> lookahead_depth;  // k: kolikaty vstupni symbol (1 = aktualni)
    std::optional<std::string> lookahead_symbol; // a: ocekavany vstupni symbol na pozici k
};