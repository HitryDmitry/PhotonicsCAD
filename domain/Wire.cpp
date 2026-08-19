#include "Wire.h"
#include "ComponentInstance.h"
#include <stdexcept>

Wire::Wire(PinInstance *startPin, PinInstance *endPin)
    : from(startPin)
    , to(endPin)
{
    if (!from || !to) {
        throw std::invalid_argument("Wire cannot have null pins");
    }

    from->addWirePtr(this);
    to->addWirePtr(this);
}

Wire::~Wire()
{
    // Удаляем себя из пинов
    if (from) {
        from->removeWirePtr(this);
    }
    if (to) {
        to->removeWirePtr(this);
    }
}

PinInstance *Wire::getOtherPin(PinInstance *pin) const
{
    if (pin == from)
        return to;
    if (pin == to)
        return from;
    return nullptr;
}

bool Wire::connectsToComponent(ComponentId id) const
{
    if (!from || !to || !from->component || !to->component) {
        return false;
    }
    return from->component->getId() == id || to->component->getId() == id;
}

bool Wire::connectsToPin(PinInstance *pin) const
{
    return pin == from || pin == to;
}