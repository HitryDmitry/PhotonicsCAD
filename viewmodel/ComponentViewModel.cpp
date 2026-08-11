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

void ComponentViewModel::modifyProperty(std::string propertyName, std::string newValue)
{
    for (auto &paramInst : mInstance->mParameters) {
        if (paramInst.at("key") == propertyName) {
            auto it = paramInst.find("default");
            if (it != paramInst.end()) {
                it->second = newValue;
            }
            break;
        }
    }
    notifyObservers();
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
