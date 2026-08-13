#include "CircuitViewModel.h"
#include "ComponentFactory.h"
#include <algorithm>

CircuitViewModel::CircuitViewModel()
    : m_circuit(std::make_unique<Circuit>())
{}

CircuitViewModel::~CircuitViewModel() = default;

Circuit *CircuitViewModel::getCircuit() const
{
    return m_circuit.get();
}

void CircuitViewModel::addComponent(const ComponentDefinition &def, double x, double y)
{
    // Создание компонента с помощью фабрики
    auto component = ComponentFactory::createComponent(def, x, y);
    ComponentInstance *rawPtr = component.get();

    // ViewModel добавляет его в модель Circuit, которой владеет
    m_circuit->components.push_back(std::move(component));

    // Создаем ComponentViewModel
    auto componentViewModel = std::make_unique<ComponentViewModel>(rawPtr);
    auto compVMRawPtr = componentViewModel.get();

    // Сохраняем его в контейнер (CircuitViewModel владеет множеством ComponentViewModel)
    mComponentVMs.push_back(std::move(componentViewModel));

    //Уведомляем подписанные представления (MainWindow)
    notifyComponentAdded(compVMRawPtr, &def);
}

void CircuitViewModel::removeComponent(ComponentViewModel *vm) {}

void CircuitViewModel::addObserver(ICircuitObserver *observer)
{
    if (observer
        && std::find(m_observers.begin(), m_observers.end(), observer) == m_observers.end()) {
        m_observers.push_back(observer);
    }
}

void CircuitViewModel::removeObserver(ICircuitObserver *observer)
{
    m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer),
                      m_observers.end());
}

void CircuitViewModel::notifyComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def)
{
    for (auto *obs : m_observers) {
        obs->onComponentAdded(cvm, def);
    }
}