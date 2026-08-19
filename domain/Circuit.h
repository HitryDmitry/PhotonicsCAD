#pragma once
#include "ComponentInstance.h"
#include "PinRef.h"
#include "Wire.h"

struct WireKey
{
    PinRef a;
    PinRef b;

    WireKey(const PinRef &p1, const PinRef &p2)
        : a(p1)
        , b(p2)
    {
        // canonical ordering по значению
        if (a > b) {
            std::swap(a, b);
        }
    }

    bool operator==(const WireKey &other) const { return a == other.a && b == other.b; }
};

template<>
struct std::hash<WireKey>
{
    size_t operator()(const WireKey &k) const
    {
        // Хэшируем значения
        size_t h1 = std::hash<ComponentId>{}(k.a.componentId)
                    ^ (std::hash<PinIndex>{}(k.a.pinIndex) << 1);
        size_t h2 = std::hash<ComponentId>{}(k.b.componentId)
                    ^ (std::hash<PinIndex>{}(k.b.pinIndex) << 1);

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

class Circuit
{
public:
    bool addComponent(std::unique_ptr<ComponentInstance> ptr);
    bool removeComponent(ComponentId id);
    bool moveComponent(ComponentId id, int newX, int newY);

    bool canConnect(const PinRef &a, const PinRef &b);
    bool addWire(const PinRef &a, const PinRef &b);
    bool removeWire(const PinRef &a, const PinRef &b);

    ComponentInstance *findComponent(ComponentId id);
    Wire *findWire(const PinRef &a, const PinRef &b);

    size_t getComponentCount() const { return mComponents.size(); }
    size_t getWireCount() const { return mWires.size(); }

    // private:
    std::vector<std::unique_ptr<ComponentInstance>> mComponents;
    std::unordered_map<WireKey, std::unique_ptr<Wire>> mWires;
};