#pragma once
#include "Circuit.h"
#include "ComponentDefinition.h"
#include "ComponentInstance.h"
#include <memory>
#include <vector>

class IPinObserver
{
public:
    virtual ~IPinObserver() = default;
    virtual void onPinClick(ComponentInstance *instance, const ComponentDefinition *def) = 0;
};

class PinViewModel
{
public:
};