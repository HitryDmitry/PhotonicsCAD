#pragma once
#include <memory>
#include <vector>
#include "Circuit.h"
#include "ComponentInstance.h"
#include "ComponentDefinition.h"

// Интерфейс наблюдателя для связи ViewModel -> View (как в Части 2 видеоуроков)
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

    // Метод добавления компонента с использованием фабрики
    void addComponent(const ComponentDefinition &def, double x, double y);

    // Доступ к модели схемы (для совместимости со старым кодом сцены)
    Circuit* getCircuit() const;

    // Методы паттерна "Наблюдатель" (как в Части 2 и 3 видеоуроков)
    void addObserver(ICircuitObserver *observer);
    void removeObserver(ICircuitObserver *observer);

private:
    void notifyComponentAdded(ComponentInstance *instance, const ComponentDefinition *def);

    std::unique_ptr<Circuit> m_circuit;
    std::vector<ICircuitObserver *> m_observers;
};