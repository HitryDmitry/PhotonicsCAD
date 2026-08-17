#pragma once
#include "ComponentId.h"
#include "PinIndex.h"

struct PinRef
{
    ComponentId componentId;
    PinIndex pinIndex;

    bool operator<(const PinRef &other) const
    {
        if (componentId != other.componentId)
            return componentId.value() < other.componentId.value();
        return pinIndex.value() < other.pinIndex.value();
    }

    bool operator>(const PinRef &other) const { return other < *this; }
};