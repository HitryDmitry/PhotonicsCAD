#pragma once
#include <cstdint>
#include <functional>

class ComponentId
{
public:
    constexpr ComponentId() = default;
    explicit constexpr ComponentId(std::uint64_t value)
        : mValue(value)
    {}

    constexpr std::uint64_t value() const noexcept { return mValue; }

    constexpr bool operator==(const ComponentId &) const = default;

    constexpr bool operator<(const ComponentId &other) const { return mValue < other.mValue; }
    constexpr bool operator>(const ComponentId &other) const { return mValue > other.mValue; }

private:
    std::uint64_t mValue{};
};

template<>
struct std::hash<ComponentId>
{
    size_t operator()(const ComponentId &id) const { return id.value(); }
};