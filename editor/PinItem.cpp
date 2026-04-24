#include "PinItem.h"
#include <QBrush>

PinItem::PinItem(PinInstance *pin, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
    , pin(pin)
{
    setRect(-5, -5, 10, 10);
    setBrush(Qt::blue);
    setZValue(10); // поверх компонента
}

PinInstance *PinItem::getPin() const
{
    return pin;
}
