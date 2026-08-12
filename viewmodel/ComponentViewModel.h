#pragma once
#include "ComponentInstance.h"

class IComponentObserver
{
public:
    virtual ~IComponentObserver() = default;
    virtual void onPropertyModyfied() = 0;
};

class ComponentViewModel
{
public:
    explicit ComponentViewModel(ComponentInstance *instance);
    void addObserver(IComponentObserver *);
    void removeObserver(IComponentObserver *);
    void modifyProperty(const std::string &propertyName, const std::string &newValue);

    const std::vector<std::map<std::string, std::string>> &getInstanceParamsVector();

private:
    void notifyObservers();
    std::vector<IComponentObserver *> mObservers;
    ComponentInstance *mInstance;
};