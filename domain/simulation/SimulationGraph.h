#pragma once

#include <unordered_map>
#include <vector>

#include "ComponentId.h"
#include "PinRef.h"

class SimulationGraph {
public:
    // Связи: Выходной пин -> список Входных пинов
    std::unordered_map<PinRef, std::vector<PinRef>> outputToInputs;

    // Обратная связь: Входной пин -> Выходной пин
    std::unordered_map<PinRef, PinRef> inputToOutput;

    // Источники сигналов
    std::vector<ComponentId> sources;

    // Точки наблюдения
    std::vector<PinRef> observationPoints;

    // Порядок вычисления компонентов
    std::vector<ComponentId> executionOrder;
};