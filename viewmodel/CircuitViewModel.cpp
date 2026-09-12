#include "CircuitViewModel.h"
#include "ComponentFactory.h"

#include <algorithm>
#include <numeric>

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

bool CircuitViewModel::removeComponent(ComponentId id)
{
    if (mCircuit->removeComponent(id)) {
        return true;
    }
    return false;
}

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

void CircuitViewModel::buildGraph()
{
    std::promise<std::unique_ptr<SimulationGraph>> graphPromise;
    mGraphFuture = graphPromise.get_future();

    std::future<std::unique_ptr<SimulationGraph>> f = std::async(std::launch::async, [this]() {
        return GraphBuilder::build(mCircuit.get());
    });

    mGraph = f.get();
    notifyGraphIsBuilt();
}

void CircuitViewModel::solveTheCircuit()
{
    std::promise<SimulationResult> simulationPromise;
    mSimulationFuture = simulationPromise.get_future();

    // --- Пока что используем костыль для сетки частот, должна задаваться в другом месте ---
    size_t numFreqs = 1e3;
    std::vector<double> frequencies(numFreqs);
    double startingFreq = 194e9;
    std::iota(frequencies.begin(), frequencies.end(), startingFreq);
    // ---

    std::future<SimulationResult> f = std::async(std::launch::async, [this, &frequencies]() {
        return mSolver.solveFrequencyDomain(*mGraph.get(), mCircuit.get(), frequencies);
    });

    mSimResult = f.get();
    notifySimulationCompleted();
}

void CircuitViewModel::notifyGraphIsBuilt()
{
    for (auto *obs : mObservers) {
        obs->onGraphBuildingCompleted();
    }
}

void CircuitViewModel::notifySimulationCompleted()
{
    for (auto *obs : mObservers) {
        obs->onSimulationCompleted(mGraph.get()->observationPoints);
    }
}

void CircuitViewModel::startSimulation()
{
    buildGraph();
    solveTheCircuit();
}
