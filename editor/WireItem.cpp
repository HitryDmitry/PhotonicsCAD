#include "WireItem.h"
#include <QPainterPath>
#include "PinItem.h"
#include <qpen.h>

WireItem::WireItem(PinItem *startPin)
    : startPin(startPin)
{
    setPen(QPen(Qt::black, 2));
}

void WireItem::setEndPoint(const QPointF &pos)
{
    tempEnd = pos;
    updatePath();
}

void WireItem::setEndPin(PinItem *pin)
{
    endPin = pin;
    updatePath();
}

void WireItem::updatePath()
{
    QPointF p1 = startPin->scenePos();
    QPointF p2 = endPin ? endPin->scenePos() : tempEnd;

    QPainterPath path(p1);

    // простая кривая (как в node editors)
    QPointF ctrl1 = p1 + QPointF(50, 0);
    QPointF ctrl2 = p2 + QPointF(-50, 0);

    path.cubicTo(ctrl1, ctrl2, p2);

    setPath(path);
}
