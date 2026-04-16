#include "GraphicsComponentItem.h"
#include <QDebug>
#include <QPixmap>

GraphicsComponentItem::GraphicsComponentItem(ComponentInstance *instance,
                                             const ComponentDefinition *def)
    : instance(instance)
{
    setPixmap(QPixmap(def->iconPath));
    setScale(0.5);

    componentType = def->type;
    setFlags(ItemIsMovable | ItemIsSelectable);
}

QVariant GraphicsComponentItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (instance == nullptr) {
        qDebug() << "Instance is nullptr!!";
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
