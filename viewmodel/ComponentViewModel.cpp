#include "ComponentViewModel.h"
#include "CircuitViewModel.h"

ComponentViewModel::ComponentViewModel(ComponentId id, CircuitViewModel *cvm, bool isInstrument)
    : mId(id)
    , mCircuitVM(cvm)
    , mIsInstrument(isInstrument)
{}

void ComponentViewModel::addObserver(IComponentObserver *observer)
{
    mObservers.push_back(observer);
}

void ComponentViewModel::removeObserver(IComponentObserver *observer)
{
    auto it = std::find(mObservers.begin(), mObservers.end(), observer);

    if (it != mObservers.end()) {
        mObservers.erase(it);
    }
}

void ComponentViewModel::modifyProperty(const std::string &propertyName, const std::string &newValue)
{
    if (mCircuitVM->getComponent(mId)->setParameter(propertyName, newValue)) {
        notifyObservers();
    }
}

double ComponentViewModel::getX() const
{
    return mCircuitVM->getComponent(mId)->getX();
}

double ComponentViewModel::getY() const
{
    return mCircuitVM->getComponent(mId)->getY();
}

const std::string &ComponentViewModel::getType()
{
    return mCircuitVM->getComponent(mId)->getType();
}

ComponentId ComponentViewModel::getId()
{
    return mId;
}

void ComponentViewModel::notifyObservers()
{
    for (auto &o : mObservers) {
        o->onPropertyModyfied();
    }
}

const std::vector<std::map<std::string, std::string> > &ComponentViewModel::getInstanceParamsVector()
{
    return mCircuitVM->getComponent(mId)->mParameters;
}

const std::vector<std::unique_ptr<PinInstance> > &ComponentViewModel::getInstancePins()
{
    return mCircuitVM->getComponent(mId)->mPins;
}

const std::vector<double> &ComponentViewModel::getSimFreqs()
{
    return mCircuitVM->getSimFreqs();
}

const std::vector<double> &ComponentViewModel::getSimVals()
{
    // В текущей реализации предполагаем, что будем получать
    // отсчеты только от наблюдающих устройств (имеют только один пин)
    PinRef pinOfIntrest{mId, PinIndex(0)};
    return mCircuitVM->getSimVals(pinOfIntrest);
}
