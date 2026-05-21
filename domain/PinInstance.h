#pragma once
#include <QSet>
#include <QString>
#include <QVariant>
#include <QVector>

class ComponentInstance;
class Wire;

class PinInstance
{
public:
    PinInstance(const QMap<QString, QVariant> &pinDef);

    QSet<Wire *> getWires();
    void addWirePtr(Wire *);

    QString id;
    QString signalType; // "optical" / "electrical"
    QString direction;  // "input" / "output"

    ComponentInstance *component = nullptr;

private:
    QSet<Wire *> wires;
};
