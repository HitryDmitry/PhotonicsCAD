#pragma once
#include "ComponentId.h"
#include "PinIndex.h"

struct PinRef
{
    ComponentId componentId;
    PinIndex pinIndex;

    bool operator==(const PinRef &other) const
    {
        return componentId.value() == other.componentId.value()
               && pinIndex.value() == other.pinIndex.value();
    }

    bool operator<(const PinRef &other) const
    {
        if (componentId != other.componentId)
            return componentId.value() < other.componentId.value();
        return pinIndex.value() < other.pinIndex.value();
    }

    bool operator>(const PinRef &other) const { return other < *this; }
};

template<>
struct std::hash<PinRef>
{
    size_t operator()(const PinRef &p) const
    {
        uint64_t combined = (static_cast<uint64_t>(p.componentId.value()) << 16)
                            | p.pinIndex.value();
        return std::hash<uint64_t>{}(combined);
    }
};