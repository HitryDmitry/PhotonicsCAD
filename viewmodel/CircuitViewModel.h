#pragma once
#include <memory>
#include <vector>
#include "Circuit.h"
#include "ComponentInstance.h"
#include "ComponentDefinition.h"

// Интерфейс наблюдателя для связи ViewModel -> View
class ICircuitObserver
{
public:
    virtual ~ICircuitObserver() = default;
    virtual void onComponentAdded(ComponentInstance *instance, const ComponentDefinition *def) = 0;
};

class CircuitViewModel
{
public:
    CircuitViewModel();
    ~CircuitViewModel();

    void addComponent(const ComponentDefinition &def, double x, double y);

    Circuit* getCircuit() const;

    void addObserver(ICircuitObserver *observer);
    void removeObserver(ICircuitObserver *observer);

private:
    void notifyComponentAdded(ComponentInstance *instance, const ComponentDefinition *def);

    std::unique_ptr<Circuit> m_circuit;
    std::vector<ICircuitObserver *> m_observers;
};