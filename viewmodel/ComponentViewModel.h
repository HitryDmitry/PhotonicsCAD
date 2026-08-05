#pragma once
#include "ComponentDefinition.h"
#include "ComponentInstance.h"
#include <memory>
#include <vector>

class IComponentObserver
{
public:
    virtual ~IComponentObserver() = default;
    virtual void onPinClick(ComponentInstance *instance, const ComponentDefinition *def) = 0;
};

class ComponentViewModel
{
public:
};