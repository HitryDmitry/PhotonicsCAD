#include "PinItem.h"
#include <QBrush>
#include "PinInstance.h"
#include <qpen.h>

PinItem::PinItem(PinInstance *pin, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
    , pin(pin)
{
    // Включаем получение событий наведения
    setAcceptHoverEvents(true);
    setRect(-5, -5, 30, 30);
    // Set the outline color (e.g., black) and width
    setPen(QPen(Qt::black, 5));
    // Set the fill to empty/transparent
    setBrush(Qt::NoBrush);
    setZValue(-1); // поверх компонента

    // разрешаем получение событий даже для прозрачных областей
    setFlag(QGraphicsItem::ItemIsSelectable);
}

PinInstance *PinItem::getPin() const
{
    return pin;
}

void PinItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mousePressEvent(event);
}

void PinItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setPen(QPen(Qt::black, 5));
    setBrush(Qt::blue);
}

void PinItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setPen(QPen(Qt::black, 5));
    setBrush(Qt::NoBrush);
}