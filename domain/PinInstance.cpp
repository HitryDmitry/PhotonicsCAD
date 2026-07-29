#include "PinInstance.h"
#include "Wire.h"

PinInstance::PinInstance(const std::map<std::string, std::string> &pinDef)
{
    // В std::map нет метода .value(), возвращающего дефолтное значение при отсутствии ключа.
    // Создан лямбда-помощник, чтобы безопасно извлекать строки без риска вызвать исключение.
    auto getValue = [&pinDef](const std::string &key) -> std::string {
        auto it = pinDef.find(key);
        return (it != pinDef.end()) ? it->second : "";
    };

    id = getValue("id");
    signalType = getValue("signal_type");
    direction = getValue("direction");
}

std::unordered_set<Wire *> PinInstance::getWires()
{
    return wires;
}

void PinInstance::addWirePtr(Wire *wirePtr)
{
    wires.insert(wirePtr);
}