#pragma once
#include <QGraphicsPixmapItem>
#include <QString>
#include <QVariant>
#include <QVector>

#include "ComponentDefinition.h"

class ComponentInstance
{
public:
    ComponentInstance(const ComponentDefinition &def);
    QString type;
    QPointF position;
    QVector<QMap<QString, QVariant>> parameters;
};