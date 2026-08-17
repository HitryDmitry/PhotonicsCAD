#include <cstdint>

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