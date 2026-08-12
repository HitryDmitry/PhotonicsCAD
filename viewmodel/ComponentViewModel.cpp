#include "ComponentViewModel.h"

ComponentViewModel::ComponentViewModel(ComponentInstance *instance)
    : mInstance(instance)
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
    if (mInstance->setParameter(propertyName, newValue)) {
        notifyObservers();
    }
}

void ComponentViewModel::notifyObservers()
{
    for (auto &o : mObservers) {
        o->onPropertyModyfied();
    }
}

const std::vector<std::map<std::string, std::string> > &ComponentViewModel::getInstanceParamsVector()
{
    return mInstance->mParameters;
}
