#pragma once
#include <unordered_set>
#include <string>
#include <map>

class ComponentInstance;
class Wire;

class PinInstance
{
public:
    // Заменил QMap<QString, QVariant> на стандартную карту строк
    PinInstance(const std::map<std::string, std::string> &pinDef);

    // Стандартное неупорядоченное множество
    std::unordered_set<Wire *> getWires();
    void addWirePtr(Wire *);

    std::string id;
    std::string signalType; // "optical" / "electrical"
    std::string direction;  // "input" / "output"

    ComponentInstance *component = nullptr;

private:
    std::unordered_set<Wire *> wires;
};