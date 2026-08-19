#pragma once
#include "ComponentId.h"
#include "PinInstance.h"

class Wire
{
public:
    Wire(PinInstance *startPin = nullptr, PinInstance *endPin = nullptr);

    ~Wire();

    PinInstance *getStartPin() const { return from; }
    PinInstance *getEndPin() const { return to; }
    PinInstance *getOtherPin(PinInstance *pin) const;

    bool connectsToComponent(ComponentId id) const;
    bool connectsToPin(PinInstance *pin) const;

    bool operator==(const Wire &other) const
    {
        return (from == other.from && to == other.to) || (from == other.to && to == other.from);
    }

    // Запрещаем копирование
    Wire(const Wire &) = delete;
    Wire &operator=(const Wire &) = delete;

private:
    PinInstance *from;
    PinInstance *to;
};
