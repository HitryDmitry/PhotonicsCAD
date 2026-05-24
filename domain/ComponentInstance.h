#pragma once
#include <QGraphicsPixmapItem>
#include <QString>
#include <QVariant>
#include <QVector>

#include "ComponentDefinition.h"
#include "PinInstance.h"

class ComponentInstance
{
public:
    ComponentInstance(const ComponentDefinition &def);
    QString type;
    QPointF position;
    QVector<QMap<QString, QVariant>> parameters;
    std::vector<std::unique_ptr<PinInstance>> pins;
};