#pragma once
#include "Circuit.h"
#include "ComponentDefinition.h"
#include "ComponentIdGenerator.h"
#include "ComponentViewModel.h"
#include "GraphBuilder.h"
#include "Solver.h"

#include <future>
#include <memory>
#include <vector>

enum CircuitState { Dragging, Completed, Simulating };

// Интерфейс наблюдателя для связи ViewModel -> View
class ICircuitObserver
{
public:
    virtual ~ICircuitObserver() = default;
    virtual void onComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def) {};
    virtual void onGraphBuildingCompleted() {};
    virtual void onSimulationCompleted(const std::vector<PinRef> &observationPoints) {};
};

class CircuitViewModel
{
public:
    CircuitViewModel();
    ~CircuitViewModel();

    void addComponent(const ComponentDefinition &def, double x, double y);
    bool removeComponent(ComponentId id);

    void addObserver(ICircuitObserver *observer);
    void removeObserver(ICircuitObserver *observer);

    ComponentInstance *getComponent(ComponentId id);
    ComponentViewModel *getComponentVM(ComponentId id);

    void changeCircuitState();
    bool checkConnectionStarted();
    bool tryToConnect(const PinRef &a, const PinRef &b);
    bool removeWire(const PinRef &a, const PinRef &b);

    // Симуляция схемы
    void startSimulation();

private:
    void notifyComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def);

    std::unordered_map<ComponentId, std::unique_ptr<ComponentViewModel>> mComponentVMs;
    std::unique_ptr<Circuit> mCircuit;
    std::vector<ICircuitObserver *> mObservers;
    ComponentIdGenerator mIdGen;

    CircuitState state{Completed};

    // Симуляция схемы
    void buildGraph();
    void solveTheCircuit();

    void notifyGraphIsBuilt();
    void notifySimulationCompleted();
    // GraphBuilder mGraphBuilder;
    std::unique_ptr<SimulationGraph> mGraph;
    Solver mSolver;
    SimulationResult mSimResult;
    std::future<std::unique_ptr<SimulationGraph>> mGraphFuture;
    std::future<SimulationResult> mSimulationFuture;
};