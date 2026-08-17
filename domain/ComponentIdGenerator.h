#include "ComponentId.h"

class ComponentIdGenerator
{
public:
    ComponentId generateNext() { return ComponentId(++m_nextId); }

    void reset() { m_nextId = 0; }

private:
    std::uint64_t m_nextId{0};
};