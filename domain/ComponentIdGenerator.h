#include "ComponentId.h"

class ComponentIdGenerator
{
public:
    ComponentId generateNext() { return ComponentId(++m_nextId); }

private:
    std::uint64_t m_nextId{0};
};