#pragma once
#include "ComponentId.h"
#include <map>
#include <memory>
#include <string>

class CircuitViewModel;

class IComponentObserver
{
public:
    virtual ~IComponentObserver() = default;
    virtual void onPropertyModyfied() = 0;
};

class ComponentViewModel
{
public:
    explicit ComponentViewModel(ComponentId id, CircuitViewModel *cvm);
    void addObserver(IComponentObserver *);
    void removeObserver(IComponentObserver *);
    void modifyProperty(const std::string &propertyName, const std::string &newValue);

    double getX() const;
    double getY() const;

    const std::string &getType();

    const ComponentId getId();

    const std::vector<std::map<std::string, std::string>> &getInstanceParamsVector();

private:
    void notifyObservers();
    std::vector<IComponentObserver *> mObservers;
    ComponentId mId;
    CircuitViewModel *mCircuitVM;
};