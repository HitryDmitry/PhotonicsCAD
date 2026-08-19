#include "Circuit.h"

bool Circuit::addComponent(std::unique_ptr<ComponentInstance> ptr)
{
    if (!ptr)
        return false; // Проверка на nullptr

    // Проверка на дубликаты ID
    if (findComponent(ptr->getId())) {
        return false;
    }

    mComponents.push_back(std::move(ptr));
    return true;
}

bool Circuit::removeComponent(ComponentId id)
{
    auto it = std::find_if(mComponents.begin(), mComponents.end(), [&id](const auto &comp) {
        return comp->getId() == id;
    });

    if (it == mComponents.end()) {
        return false;
    }

    // Удаляем все провода, связанные с этим компонентом
    // (это требует дополнительной логики)
    mComponents.erase(it);
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

bool Circuit::addWire(const PinRef &a, const PinRef &b)
{
    WireKey newWireKey(a, b);

    // Проверка существования
    if (mWires.contains(newWireKey)) {
        return false;
    }

    // Проверка компонентов и пинов
    auto compA = findComponent(a.componentId);
    auto compB = findComponent(b.componentId);

    if (!compA || !compB) {
        return false;
    }

    if (!canConnect(a, b)) {
        return false;
    }

    auto pinA = compA->findPin(a.pinIndex);
    auto pinB = compB->findPin(b.pinIndex);

    // Создаем и сохраняем провод с ключом
    auto wire = std::make_unique<Wire>(&pinA, &pinB);
    mWires.emplace(newWireKey, std::move(wire));

    return true;
}

bool Circuit::removeWire(const PinRef &a, const PinRef &b)
{
    WireKey key(a, b);

    auto it = mWires.find(key);
    if (it == mWires.end()) {
        return false;
    }

    mWires.erase(it);
    return true;
}

Wire *Circuit::findWire(const PinRef &a, const PinRef &b)
{
    WireKey key(a, b);
    auto it = mWires.find(key);
    return (it != mWires.end()) ? it->second.get() : nullptr;
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