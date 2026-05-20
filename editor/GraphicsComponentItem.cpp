#include "GraphicsComponentItem.h"
#include <QDebug>
#include <QPixmap>
#include "PinItem.h"

GraphicsComponentItem::GraphicsComponentItem(ComponentInstance *instance,
                                             const ComponentDefinition *def)
    : instance(instance)
{
    setPixmap(QPixmap(def->iconPath));
    setScale(0.25);

    componentType = def->type;
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);

    createPins(def);
}

QVariant GraphicsComponentItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsPixmapItem::itemChange(change, value);
}

ComponentInstance *GraphicsComponentItem::getInstance()
{
    return instance;
}

QVector<PinItem *> GraphicsComponentItem::getPins()
{
    return pins;
}

void GraphicsComponentItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit doubleClicked(instance);
    QGraphicsPixmapItem::mouseDoubleClickEvent(event);
}

void GraphicsComponentItem::createPins(const ComponentDefinition *def)
{
    int count = def->pins.size();

    for (int i = 0; i < count; i++) {
        auto pinInst = new PinInstance(def->pins.at(i));
        pinInst->component = instance;

        instance->pins.push_back(pinInst);

        auto pinItem = new PinItem(pinInst, this);

        pins.push_back(pinItem);

        if (i % 2 == 0)
            pinItem->setPos(-100, i * 100);
        else
            pinItem->setPos(900, i * 100);
    }
}
