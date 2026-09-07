#include "Solver.h"
#include "CircuitBuilder.h"
#include "Constants.h"
#include "GraphBuilder.h"

#include <doctest/doctest.h>

TEST_CASE("LinearSolver - Simple chain of components")
{
    CircuitBuilder builder;
    Solver linearSolver;

    // Создаем схему: Laser -> Fiber -> Photodetector
    auto laserId = builder.addComponent("laser");
    auto fiberId = builder.addComponent("optical_fiber");
    auto pdId = builder.addComponent("photodetector");

    builder.addWire(laserId, "out", fiberId, "in");
    builder.addWire(fiberId, "out", pdId, "opt_in");

    // Получаем уникальный указатель на схему
    auto circuit = builder.build();
    // Вытаскиваем из него сырой указатель
    auto circuitPtr = circuit.get();
    // Уникальный указатель на граф
    auto graph = GraphBuilder::build(circuit.get());
    auto graphPtr = graph.get();

    // Формируем массив частот [194, 194.5, 195 ГГц] на которых будет рассчитан результат
    std::vector<double> frequencies{193 * Units::GHz, 193.5 * Units::GHz, 194 * Units::GHz};

    // Вход фотодетектора
    PinRef pdInput{pdId, builder.getPinIndex(pdId, "opt_in")};
    // Добавляем его в точки наблюдения
    graphPtr->observationPoints.push_back(pdInput);

    // Вычисляем результат на этих частотах на входе фотодетектора
    auto simResult = linearSolver.solveFrequencyDomain(*graphPtr, circuitPtr, frequencies);

    // Значения на заданных частотах на входе фотодетектора (расчетные и эталонные)
    std::vector<std::complex<double>> pdInputCalculated = simResult.frequencyResponses.at(pdInput);
    std::vector<std::complex<double>> pdInputEtalonVals{{0, 0}, {1, 0}, {0, 0}};

    // Значение мощности на входе фотодетектора
    for (size_t idx = 0; idx < pdInputEtalonVals.size(); idx++) {
        CHECK(pdInputCalculated[idx] == pdInputEtalonVals[idx]);
    }
}