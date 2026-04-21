#pragma once
#include <QGraphicsPixmapItem>

#include "ComponentDefinition.h"
#include "ComponentInstance.h"

class GraphicsComponentItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    GraphicsComponentItem(ComponentInstance *instance, const ComponentDefinition *def);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    ComponentInstance *getInstance();
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    QString componentType;
    ComponentInstance *instance;

signals:
    void doubleClicked(ComponentInstance *instance);
};
