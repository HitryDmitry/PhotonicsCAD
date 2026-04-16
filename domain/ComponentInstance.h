#pragma once
#include <QGraphicsPixmapItem>
#include <QString>
#include <QVariant>

#include "ComponentDefinition.h"

class ComponentInstance
{
public:
    ComponentInstance(const ComponentDefinition &def);
    QString type;
    QPointF position;
    QMap<QString, QVariant> parameters;
};