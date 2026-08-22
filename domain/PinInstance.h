#pragma once
#include <unordered_set>
#include <string>
#include <map>
#include <unordered_set>
#include <string>
#include <map>

#include "PinIndex.h"

class ComponentInstance;
class Wire;

class PinInstance
{
public:
    PinInstance(const std::map<std::string, std::string> &pinDef, PinIndex idx);

    const std::unordered_set<Wire *> &getWires() const { return wires; }
    size_t getWireCount() const { return wires.size(); }
    bool isConnected() const { return !wires.empty(); }

    bool addWirePtr(Wire *wirePtr);
    bool removeWirePtr(Wire *wirePtr);
    bool hasWire(Wire *wirePtr) const;
    void clearWires();

    void setComponent(ComponentInstance *comp);
    PinIndex getPinIdx() { return mIdx; };
    const std::string &getSignalType() { return signalType; };
    const std::string &getDirection() { return direction; };
    const ComponentInstance *getComponent() { return component; };

private:
    std::unordered_set<Wire *> wires;
    PinIndex mIdx;
    std::string id;         // "in" / "out"
    std::string signalType; // "optical" / "electrical"
    std::string direction;  // "input" / "output"
    ComponentInstance *component = nullptr;
};