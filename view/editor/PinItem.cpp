#include "PinItem.h"
#include <QBrush>
#include "ComponentViewModel.h"
#include "WireItem.h"
#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qpen.h>

PinItem::PinItem(QGraphicsItem *parent, ComponentViewModel *cvm, PinIndex idx)
    : QGraphicsEllipseItem(parent)
    , mCompVM(cvm)
    , mIdx(idx)
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
    mWireItems.insert(wire);
}

void PinItem::removeWire(WireItem *wire)
{
    if (mWireItems.remove(wire)) {
        qDebug() << "WireItem was removed.";
    } else {
        qDebug() << "Can't remove WireItem!";
    }
}

const QSet<WireItem *> &PinItem::getWireItems()
{
    return mWireItems;
}

ComponentId PinItem::getComponentId()
{
    return mCompVM->getId();
}

PinIndex PinItem::getPinIdx()
{
    return mIdx;
}

PinRef PinItem::getPinRef()
{
    return PinRef(getComponentId(), getPinIdx());
}

void PinItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit pinClicked(this);
    event->accept();
}