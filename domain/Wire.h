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
        // Комбинирование хэшей с перемешиванием для лучшего распределения
        size_t h1 = qHash(wire.from, seed);
        size_t h2 = qHash(wire.to, seed);
        return h1 ^ h2;
    }

    PinInstance *from;
    PinInstance *to;
};
