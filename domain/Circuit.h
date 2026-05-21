#include "ComponentInstance.h"
#include "Wire.h"

class Circuit
{
public:
    std::vector<std::unique_ptr<ComponentInstance>> components;
    std::vector<std::unique_ptr<Wire>> wires;
};
