#pragma once

#include "ComponentInstance.h"
#include "Wire.h"
#include <memory>

struct WireKey
{
    const PinRef *a;
    const PinRef *b;

    WireKey(const PinRef *p1, const PinRef *p2)
    {
        // canonical ordering
        if (p1 < p2) {
            a = p1;
            b = p2;
        } else {
            a = p2;
            b = p1;
        }
    }

    bool operator==(const WireKey &other) const { return a == other.a && b == other.b; }
};

namespace std {
template<>
struct hash<WireKey>
{
    size_t operator()(const WireKey &k) const
    {
        size_t h1 = std::hash<const void *>{}(k.a);
        size_t h2 = std::hash<const void *>{}(k.b);

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
} // namespace std

class Circuit
{
public:
    bool addComponent();
    bool removeComponent();
    bool moveComponent();

    bool canConnect(const PinRef &a, const PinRef &b);
    bool addWire(const PinRef &a, const PinRef &b);
    bool removeWire(const PinRef &a, const PinRef &b);

    ComponentInstance *findComponent(ComponentId id);

    // private:
    std::vector<std::unique_ptr<ComponentInstance>> mComponents;
    std::vector<std::unique_ptr<Wire>> mWires;

    std::unordered_set<WireKey> mWireIdxs;
};