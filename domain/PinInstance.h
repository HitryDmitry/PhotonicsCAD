#include <QString>
#include <QVariant>
#include <QVector>

class ComponentInstance;

class PinInstance
{
public:
    PinInstance(const QMap<QString, QVariant> &pinDef);

    QString id;
    QString signalType; // "optical" / "electrical"
    QString direction;  // "input" / "output"

    ComponentInstance *component = nullptr;
};
