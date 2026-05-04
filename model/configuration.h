#pragma once

#include <string>
#include <vector>
#include <memory>

enum class StepType {
    Start,
    Pop,
    Expand,
};

struct Configuration {
    std::string state; // aktualni stav
    size_t input_pos; // pozice ve vstupu
    std::vector<std::string> stack; // zasobnik (vrchol -> .front())

    std::shared_ptr<Configuration> parent; // predchozi konfigurace
    size_t applied_rule; // index pravidla
    StepType step_type = StepType::Start; // jak konfigurace vznikla
};