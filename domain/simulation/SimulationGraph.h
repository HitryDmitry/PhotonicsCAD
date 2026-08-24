#pragma once

#include <vector>
#include <unordered_map>
#include <functional> // Для std::hash

// Подключаем ваши строгие типы и структуру PinRef
#include "PinRef.h"
#include "ComponentId.h"
#include "Circuit.h"

// --- Учим C++ хешировать вашу обертку PinRef ---
struct PinRefHash {
    std::size_t operator()(const PinRef& p) const {
        // Достаем внутренние значения через .value() и хешируем их
        auto h1 = std::hash<decltype(p.componentId.value())>{}(p.componentId.value());
        auto h2 = std::hash<decltype(p.pinIndex.value())>{}(p.pinIndex.value());
        return h1 ^ (h2 << 1);
    }
};
// ------------------------------------------------

class SimulationGraph {
public:
    // 4) Связи: Выходной пин -> список Входных пинов (ПЕРЕДАЕМ НАШ ХЕШ)
    std::unordered_map<PinRef, std::vector<PinRef>, PinRefHash> outputToInputs;

    // 5) Обратная связь: Входной пин -> Выходной пин (ПЕРЕДАЕМ НАШ ХЕШ)
    std::unordered_map<PinRef, PinRef, PinRefHash> inputToOutput;

    // 6) Источники сигналов (используем ВАШ тип ComponentId)
    std::vector<ComponentId> sources;

    // 7) Точки наблюдения
    std::vector<PinRef> observationPoints;

    // 8) Порядок вычисления компонентов (используем ВАШ тип ComponentId)
    std::vector<ComponentId> executionOrder;
};