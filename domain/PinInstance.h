#pragma once
#include <unordered_set>
#include <string>
#include <map>
#include <unordered_set>
#include <string>
#include <map>

#include "ComponentId.h"
#include "PinIndex.h"

struct PinRef
{
    ComponentId componentId;
    PinIndex pinIndex;
};

class ComponentInstance;
class Wire;

class PinInstance
{
public:
    PinInstance(const std::map<std::string, std::string> &pinDef);

    std::unordered_set<Wire *> getWires();
    void addWirePtr(Wire *);

    std::string id;
    std::string signalType; // "optical" / "electrical"
    std::string direction;  // "input" / "output"

    ComponentInstance *component = nullptr;

private:
    std::unordered_set<Wire *> wires;
};