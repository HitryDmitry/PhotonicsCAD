#include <cstdint>

class ComponentId
{
public:
    constexpr ComponentId() = default;
    explicit constexpr ComponentId(std::uint64_t value)
        : mValue(value)
    {}

    constexpr std::uint64_t value() const noexcept { return mValue; }

    constexpr bool operator==(const ComponentId &) const = default;

private:
    std::uint64_t mValue{};
};