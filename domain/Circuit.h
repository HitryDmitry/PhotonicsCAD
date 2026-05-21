#include "ComponentInstance.h"
#include "Wire.h"

class Circuit
{
public:
    QVector<ComponentInstance *> components;
    QVector<Wire *> wires;
};
