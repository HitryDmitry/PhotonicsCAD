#include "Circuit.h"

bool Circuit::addComponent(std::unique_ptr<ComponentInstance> ptr)
{
    mComponents.push_back(std::move(ptr));
    return true;
}

bool Circuit::canConnect(const PinRef &a, const PinRef &b)
{
    auto compA = findComponent(a.componentId);
    auto compB = findComponent(b.componentId);

    auto pinA = compA->findPin(a.pinIndex);
    auto pinB = compB->findPin(b.pinIndex);

    if (pinA.signalType != pinB.signalType)
        return false;

    if (pinA.direction == pinB.direction)
        return false;

    if (pinA.component == pinB.component)
        return false;

    return true;
}

ComponentInstance *Circuit::findComponent(ComponentId id)
{
    for (auto &component : mComponents) {
        if (component->getId() == id) {
            return component.get();
        }
    }
    return nullptr;
}