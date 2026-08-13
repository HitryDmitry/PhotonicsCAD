#include <cstdint>

class PinIndex
{
public:
    explicit constexpr PinIndex(std::uint16_t value)
        : mValue(value)
    {}

private:
    std::uint16_t mValue{};
};