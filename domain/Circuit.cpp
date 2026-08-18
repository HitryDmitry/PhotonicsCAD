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
    // Проверяем существование компонентов
    auto compA = findComponent(a.componentId);
    auto compB = findComponent(b.componentId);

    if (!compA || !compB) {
        return false;
    }

    // Проверяем возможность соединения
    if (!canConnect(a, b)) {
        return false;
    }

    // Проверяем дубликаты
    WireKey key(a, b);
    if (mWireIdxs.find(key) != mWireIdxs.end()) {
        return false;
    }

    // Создаем провод
    auto pinA = compA->findPin(a.pinIndex);
    auto pinB = compB->findPin(b.pinIndex);

    mWireIdxs.insert(key);
    mWires.push_back(std::make_unique<Wire>(&pinA, &pinB));

    return true;
}

bool Circuit::removeWire(const PinRef &a, const PinRef &b)
{
    WireKey key(a, b);

    auto it = mWireIdxs.find(key);
    if (it == mWireIdxs.end()) {
        return false;
    }

    // Находим и удаляем соответствующий провод
    // (сложно, т.к. нет прямой связи между WireKey и Wire)
    // Требуется дополнительное поле или другая структура данных

    mWireIdxs.erase(it);
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