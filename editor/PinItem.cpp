#include "PinItem.h"
#include <QBrush>
#include "PinInstance.h"
#include "WireItem.h"
#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qpen.h>

PinItem::PinItem(PinInstance *pin, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
    , pin(pin)
{
    // Включаем получение событий наведения
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);

    setRect(-5, -5, 30, 30);
    // Set the outline color (e.g., black) and width
    setPen(QPen(Qt::black, 5));
    // Set the fill to empty/transparent
    setBrush(Qt::NoBrush);
}

PinInstance *PinItem::getPin() const
{
    return pin;
}

void PinItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit connectionStarted(this);
    event->accept();
}