
#include "PinInstance.h"
#include "Wire.h"


PinInstance::PinInstance(const QMap<QString, QVariant> &pinDef)
{
    id = pinDef.value(QString("id")).toString();
    signalType = pinDef.value(QString("signal_type")).toString();
    direction = pinDef.value(QString("direction")).toString();
}

QSet<Wire *> PinInstance::getWires()
{
    return wires;
}

void PinInstance::addWirePtr(Wire *wirePtr)
{
    wires.insert(wirePtr);
}