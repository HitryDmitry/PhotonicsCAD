#include "CircuitViewModel.h"
#include "ComponentFactory.h"
#include <algorithm>

CircuitViewModel::CircuitViewModel()
    : mCircuit(std::make_unique<Circuit>())
{}

CircuitViewModel::~CircuitViewModel() = default;

void CircuitViewModel::addComponent(const ComponentDefinition &def, double x, double y)
{
    auto newCompId = mIdGen.generateNext();

    // Создание компонента с помощью фабрики
    auto component = ComponentFactory::createComponent(def, newCompId, x, y);

    // ViewModel добавляет его в модель Circuit, которой владеет
    mCircuit->addComponent(std::move(component));

    // Создаем ComponentViewModel
    auto componentViewModel = std::make_unique<ComponentViewModel>(newCompId, this);
    auto compVMRawPtr = componentViewModel.get();

    // Сохраняем его в контейнер (CircuitViewModel владеет множеством ComponentViewModel)
    mComponentVMs.emplace(newCompId, std::move(componentViewModel));

    //Уведомляем подписанные представления (MainWindow)
    notifyComponentAdded(compVMRawPtr, &def);
}

void CircuitViewModel::removeComponent(ComponentViewModel *vm) {}

void CircuitViewModel::addObserver(ICircuitObserver *observer)
{
    if (observer && std::find(mObservers.begin(), mObservers.end(), observer) == mObservers.end()) {
        mObservers.push_back(observer);
    }
}

void CircuitViewModel::removeObserver(ICircuitObserver *observer)
{
    mObservers.erase(std::remove(mObservers.begin(), mObservers.end(), observer), mObservers.end());
}

ComponentInstance *CircuitViewModel::getComponent(ComponentId id)
{
    return mCircuit->findComponent(id);
}

ComponentViewModel *CircuitViewModel::getComponentVM(ComponentId id)
{
    auto it = mComponentVMs.find(id);

    if (it != mComponentVMs.end()) {
        return it->second.get();
    }
    return nullptr;
}

void CircuitViewModel::changeCircuitState()
{
    if (state == CircuitState::Completed) {
        state = CircuitState::Dragging;
    } else {
        state = CircuitState::Completed;
    }
}

bool CircuitViewModel::checkConnectionStarted()
{
    if (state == CircuitState::Dragging)
        return true;
    return false;
}

bool CircuitViewModel::tryToConnect(const PinRef &a, const PinRef &b)
{
    if (mCircuit->addWire(a, b)) {
        changeCircuitState();
        return true;
    }
    return false;
}

bool CircuitViewModel::removeWire(const PinRef &a, const PinRef &b)
{
    if (mCircuit->removeWire(a, b)) {
        return true;
    }
    return false;
}

void CircuitViewModel::notifyComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def)
{
    for (auto *obs : mObservers) {
        obs->onComponentAdded(cvm, def);
    }
}