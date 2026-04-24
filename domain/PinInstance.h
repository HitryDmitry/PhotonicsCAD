#include <QString>

class ComponentInstance;

class PinInstance
{
public:
    QString id;
    QString signalType; // "optical" / "electrical"
    QString direction;  // "input" / "output"

    ComponentInstance *component = nullptr;
};
