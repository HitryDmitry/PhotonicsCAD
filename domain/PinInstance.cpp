#include "PinInstance.h"
#include "Wire.h"

PinInstance::PinInstance(const std::map<std::string, std::string> &pinDef)
{
    // В std::map нет метода .value(), возвращающего дефолтное значение при отсутствии ключа.
    // Напишем лямбду-помощник, чтобы безопасно извлекать строки без риска вызвать исключение.
    auto getValue = [&pinDef](const std::string &key) -> std::string {
        auto it = pinDef.find(key);
        return (it != pinDef.end()) ? it->second : "";
    };

    id = getValue("id");
    signalType = getValue("signal_type");
    direction = getValue("direction");
}

bool PinInstance::addWirePtr(Wire *wirePtr)
{
    if (!wirePtr)
        return false;

    // Проверка на дубликат
    if (wires.find(wirePtr) != wires.end()) {
        return false;
    }

    wires.insert(wirePtr);
    return true;
}

bool PinInstance::removeWirePtr(Wire *wirePtr)
{
    if (!wirePtr)
        return false;

    auto it = wires.find(wirePtr);
    if (it == wires.end()) {
        return false;
    }

    wires.erase(it);
    return true;
}

bool PinInstance::hasWire(Wire *wirePtr) const
{
    return wires.find(wirePtr) != wires.end();
}

void PinInstance::clearWires()
{
    wires.clear();
}

void PinInstance::setComponent(ComponentInstance *comp)
{
    component = comp;
}