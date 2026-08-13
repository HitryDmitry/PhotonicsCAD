#include "PinItem.h"
#include <QBrush>
#include "ComponentViewModel.h"
#include "WireItem.h"
#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qpen.h>

PinItem::PinItem(ComponentViewModel *cvm, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
    , mCompVM(cvm)
{
    // Включаем получение событий наведения
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);

    setRect(-5, -5, 30, 30);
    // Set the outline color (e.g., black) and width
    setPen(QPen(Qt::black, 5));
    // Set the fill to empty/transparent
    setBrush(Qt::NoBrush);
    setZValue(10.0);
}

void PinItem::addWire(WireItem *wire)
{
    wireItems.insert(wire);
}

const QSet<WireItem *> &PinItem::getWireItems()
{
    return wireItems;
}

void PinItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit connectionStarted(this);
    event->accept();
}