#include "ComponentInstance.h"
#include "Wire.h"

// struct UniquePtrWireHash
// {
//     size_t operator()(const std::unique_ptr<Wire> &w) const
//     {
//         auto [first, second] = std::minmax(w->from, w->to);
//         return std::hash<PinInstance *>{}(first) ^ (std::hash<PinInstance *>{}(second) << 1);
//     }
// };

// struct UniquePtrWireEqual
// {
//     bool operator()(const std::unique_ptr<Wire> &a, const std::unique_ptr<Wire> &b) const
//     {
//         return (a->from == b->from && a->to == b->to) || (a->from == b->to && a->to == b->from);
//     }
// };

struct WireHash
{
    size_t operator()(const Wire &w) const
    {
        auto [first, second] = std::minmax(w.from, w.to);
        return std::hash<PinInstance *>{}(first) ^ (std::hash<PinInstance *>{}(second) << 1);
    }
};

struct WireEqual
{
    bool operator()(const Wire &a, const Wire &b) const
    {
        return (a.from == b.from && a.to == b.to) || (a.from == b.to && a.to == b.from);
    }
};

class Circuit
{
public:
    // Добавление провода
    bool addWire(PinInstance *from, PinInstance *to)
    {
        auto [it, inserted] = wires.emplace(from, to);
        return inserted; // true если добавили, false если уже был
    }

    // Проверка существования
    bool hasWire(PinInstance *from, PinInstance *to) const
    {
        return wires.find(Wire(from, to)) != wires.end();
    }

    // Удаление по пинам
    bool removeWire(PinInstance *from, PinInstance *to) { return wires.erase(Wire(from, to)) > 0; }

    // Удаление всех проводов, связанных с пином
    void removeWiresConnectedTo(PinInstance *pin)
    {
        std::erase_if(wires, [pin](const Wire &wire) { return wire.from == pin || wire.to == pin; });
    }

    // Получение всех проводов (только для чтения)
    const std::unordered_set<Wire, WireHash, WireEqual> &getWires() const { return wires; }

    // Очистка всех проводов
    void clear() { wires.clear(); }

    // Количество проводов
    size_t wireCount() const { return wires.size(); }

    std::vector<std::unique_ptr<ComponentInstance>> components;
    std::unordered_set<Wire, WireHash, WireEqual> wires;
};
