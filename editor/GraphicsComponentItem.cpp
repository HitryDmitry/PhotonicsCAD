#include "GraphicsComponentItem.h"
#include <QDebug>
#include <QPixmap>
#include "PinItem.h"

GraphicsComponentItem::GraphicsComponentItem(ComponentInstance *instance,
                                             const ComponentDefinition *def)
    : instance(instance)
{
    setPixmap(QPixmap(def->iconPath));
    setScale(0.5);

    componentType = def->type;
    setFlags(ItemIsMovable | ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);

    createPins(def);
}

QVariant GraphicsComponentItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (instance == nullptr) {
        qDebug() << "Instance is nullptr!";
    }
    if (change == ItemPositionChange && instance) {
        instance->position = value.toPointF();
    }

    else if (change == ItemScenePositionHasChanged && instance) {
        qDebug() << "New position: " << instance->position;
        qDebug() << "Current position: " << pos();
    }

    return QGraphicsPixmapItem::itemChange(change, value);
}

ComponentInstance *GraphicsComponentItem::getInstance()
{
    return instance;
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
        auto pinInst = new PinInstance();
        pinInst->component = instance;

        instance->pins.push_back(pinInst);

        auto pinItem = new PinItem(pinInst, this);

        // размещаем слева/справа
        if (i % 2 == 0)
            pinItem->setPos(80, i * 100);
        else
            pinItem->setPos(700, i * 100);
    }
}
