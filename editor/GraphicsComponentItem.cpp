#include "GraphicsComponentItem.h"
#include <QDebug>
#include <QPixmap>
#include "PinItem.h"
#include "WireItem.h"

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
    for (const auto &pinItemIter : std::as_const(pins)) {
        for (const auto &wireItemIter : pinItemIter->getWireItems()) {
            wireItemIter->updatePath();
        }
    }
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
        auto pinInst = std::make_unique<PinInstance>(def->pins.at(i));
        pinInst->component = instance;

        PinInstance *pinPtr = pinInst.get();

        instance->pins.push_back(std::move(pinInst));

        auto pinItem = new PinItem(pinPtr, this);

        pins.push_back(pinItem);

        if (i % 2 == 0)
            pinItem->setPos(-100, i * 100);
        else
            pinItem->setPos(900, i * 100);
    }
}
