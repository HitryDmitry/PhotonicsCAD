#pragma once

#include <memory>
#include "SimulationGraph.h"
#include "Circuit.h"

class GraphBuilder {
public:
    // Главный метод: принимает схему и строит математический граф
    static std::unique_ptr<SimulationGraph> build(const Circuit* circuit);
};