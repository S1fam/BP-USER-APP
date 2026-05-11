/**
 * @file json_loader.h
 * @author Jaroslav Ištvan (xistva03)
 * @date 2026
 * @brief Definice tridy pro nacteni automatu z JSON souboru
 */

#pragma once
#include "automaton.h"
#include <string>

class JsonLoader {
public:
    static Automaton loadFromFile(const std::string& filename);
};