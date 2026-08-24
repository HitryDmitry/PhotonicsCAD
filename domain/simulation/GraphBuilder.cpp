#include "GraphBuilder.h"
#include <queue>
#include <map>
#include <stdexcept>

std::unique_ptr<SimulationGraph> GraphBuilder::build(const Circuit* circuit) {
    if (!circuit) {
        throw std::invalid_argument("Circuit is null");
    }

    auto graph = std::make_unique<SimulationGraph>();

    std::map<ComponentId, std::vector<ComponentId>> compAdjacency;
    std::map<ComponentId, int> inDegrees;

    for (const auto& compPtr : circuit->mComponents) {
        inDegrees[compPtr->getId()] = 0;
    }

    for (const auto& [wireKey, wirePtr] : circuit->mWires) {
        PinRef pinA = wireKey.a;
        PinRef pinB = wireKey.b;

        ComponentInstance* compA = const_cast<Circuit*>(circuit)->findComponent(pinA.componentId);
        ComponentInstance* compB = const_cast<Circuit*>(circuit)->findComponent(pinB.componentId);

        if (!compA || !compB) continue;

        const PinInstance& pInstA = compA->getPin(pinA.pinIndex);
        const PinInstance& pInstB = compB->getPin(pinB.pinIndex);

        // Используем метод-геттер для получения направления
        std::string dirA = pInstA.getDirection();
        std::string dirB = pInstB.getDirection();

        bool aIsOutput = (dirA == "output" || dirA == "out" || dirA == "OUTPUT");
        bool bIsOutput = (dirB == "output" || dirB == "out" || dirB == "OUTPUT");

        if (aIsOutput && !bIsOutput) {
            graph->outputToInputs[pinA].push_back(pinB);

            // Используем метод insert, чтобы не вызывать пустой конструктор PinRef
            graph->inputToOutput.insert({pinB, pinA});

            compAdjacency[pinA.componentId].push_back(pinB.componentId);
            inDegrees[pinB.componentId]++;
        }
        else if (bIsOutput && !aIsOutput) {
            graph->outputToInputs[pinB].push_back(pinA);

            graph->inputToOutput.insert({pinA, pinB});

            compAdjacency[pinB.componentId].push_back(pinA.componentId);
            inDegrees[pinA.componentId]++;
        }
    }

    std::queue<ComponentId> queue;
    for (const auto& pair : inDegrees) {
        if (pair.second == 0) {
            queue.push(pair.first);
            graph->sources.push_back(pair.first);
        }
    }

    while (!queue.empty()) {
        ComponentId current = queue.front();
        queue.pop();

        graph->executionOrder.push_back(current);

        for (const auto& neighbor : compAdjacency[current]) {
            inDegrees[neighbor]--;
            if (inDegrees[neighbor] == 0) {
                queue.push(neighbor);
            }
        }
    }

    if (graph->executionOrder.size() != inDegrees.size() && !inDegrees.empty()) {
        throw std::runtime_error("Graph contains cycles! Sequential simulation is impossible.");
    }

    return graph;
}