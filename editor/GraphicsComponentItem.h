#pragma once
#include <QGraphicsPixmapItem>

#include "ComponentDefinition.h"
#include "ComponentInstance.h"

class PinItem;

class GraphicsComponentItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    GraphicsComponentItem(ComponentInstance *instance, const ComponentDefinition *def);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void createPins(const ComponentDefinition *def);

    ComponentInstance *getInstance();
    QVector<PinItem *> getPins();

    QString componentType;

signals:
    void doubleClicked(ComponentInstance *instance);

private:
    QVector<PinItem *> pins;
    ComponentInstance *instance;
};
