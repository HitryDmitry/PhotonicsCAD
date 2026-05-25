#include "ComponentInstance.h"
#include "Wire.h"
#include <algorithm>
#include <memory>

struct WireKey
{
    const PinInstance *a;
    const PinInstance *b;

    WireKey(const PinInstance *p1, const PinInstance *p2)
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
    std::vector<std::unique_ptr<ComponentInstance>> components;
    std::vector<std::unique_ptr<Wire>> wires;

    std::unordered_set<WireKey> wireIndex;
};