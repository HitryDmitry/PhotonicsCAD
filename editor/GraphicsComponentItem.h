#pragma once
#include <QGraphicsPixmapItem>

#include "ComponentDefinition.h"
#include "ComponentInstance.h"

class GraphicsComponentItem : public QGraphicsPixmapItem
{
public:
    GraphicsComponentItem(ComponentInstance *instance, const ComponentDefinition *def);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    ComponentInstance *getInstance();

    QString componentType;
    ComponentInstance *instance;
};
