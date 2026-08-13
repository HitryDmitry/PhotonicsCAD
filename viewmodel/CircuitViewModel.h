#pragma once
#include "Circuit.h"
#include "ComponentDefinition.h"
#include "ComponentViewModel.h"
#include <memory>
#include <vector>

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

    Circuit* getCircuit() const;

    void addObserver(ICircuitObserver *observer);
    void removeObserver(ICircuitObserver *observer);

private:
    void notifyComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def);

    std::vector<std::unique_ptr<ComponentViewModel>> mComponentVMs;
    std::unique_ptr<Circuit> m_circuit;
    std::vector<ICircuitObserver *> m_observers;
};