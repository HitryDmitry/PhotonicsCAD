#include "PinInstance.h"

PinInstance::PinInstance(const QMap<QString, QVariant> &pinDef)
{
    id = pinDef.value(QString("id")).toString();
    signalType = pinDef.value(QString("signal_type")).toString();
    direction = pinDef.value(QString("direction")).toString();
}