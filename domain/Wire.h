#pragma once
#include <QHash>
#include "PinInstance.h"

class Wire
{
public:
    Wire(PinInstance *startPin = nullptr, PinInstance *endPin = nullptr)
        : from(startPin)
        , to(endPin)
    {}

    bool operator==(const Wire &other) const
    {
        return (from == other.from && to == other.to) || (from == other.to && to == other.from);
    }

    friend size_t qHash(const Wire &wire, size_t seed = 0)
    {
        // Приводим к каноническому виду: меньший указатель первым
        auto [first, second] = std::minmax(wire.from, wire.to);
        return qHash(first, seed) ^ (qHash(second, seed) << 1);
    }

    friend size_t qHash(Wire *wire, size_t seed = 0)
    {
        // Приводим к каноническому виду: меньший указатель первым
        auto [first, second] = std::minmax(wire->from, wire->to);
        return qHash(first, seed) ^ (qHash(second, seed) << 1);
    }

    PinInstance *from;
    PinInstance *to;
};
