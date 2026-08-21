#pragma once
#include "Circuit.h"
#include "ComponentDefinition.h"
#include "ComponentIdGenerator.h"
#include "ComponentViewModel.h"
#include <memory>
#include <vector>

enum CircuitState { Dragging, Completed };

// Интерфейс наблюдателя для связи ViewModel -> View
class ICircuitObserver
{
public:
    virtual ~ICircuitObserver() = default;
    virtual void onComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def) = 0;
};

class CircuitViewModel
{
public:
    CircuitViewModel();
    ~CircuitViewModel();

    void addComponent(const ComponentDefinition &def, double x, double y);
    void removeComponent(ComponentViewModel *vm);

    void addObserver(ICircuitObserver *observer);
    void removeObserver(ICircuitObserver *observer);

    ComponentInstance *getComponent(ComponentId id);
    ComponentViewModel *getComponentVM(ComponentId id);

private:
    void notifyComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def);

    std::unordered_map<ComponentId, std::unique_ptr<ComponentViewModel>> mComponentVMs;
    std::unique_ptr<Circuit> mCircuit;
    std::vector<ICircuitObserver *> mObservers;
    ComponentIdGenerator mIdGen;

    CircuitState state{Completed};
};