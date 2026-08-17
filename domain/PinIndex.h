#pragma once
#include <cstdint>
#include <functional>

class PinIndex
{
public:
    explicit constexpr PinIndex(std::uint16_t value)
        : mValue(value)
    {}

    constexpr std::uint16_t value() const noexcept { return mValue; }

private:
    std::uint16_t mValue{};
};

template<>
struct std::hash<PinIndex>
{
    size_t operator()(const PinIndex &idx) const { return idx.value(); }
};