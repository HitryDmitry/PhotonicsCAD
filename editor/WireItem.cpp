#include "WireItem.h"
#include <QPainterPath>
#include "PinItem.h"
#include <qpen.h>

WireItem::WireItem(PinItem *startPin)
    : startPin(startPin)
{
    setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
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

    qreal dx = p2.x() - p1.x();
    qreal dy = p2.y() - p1.y();

    if (!dx) {
        plotNormalToX = false;
    }
    if (!dy) {
        plotNormalToX = true;
    }

    if (plotNormalToX) {
        // сначала по X
        QPointF mid(p2.x(), p1.y());
        path.lineTo(mid);
    } else {
        // сначала по Y
        QPointF mid(p1.x(), p2.y());
        path.lineTo(mid);
    }

    path.lineTo(p2);

    setPath(path);
}

PinItem *WireItem::getStartPin()
{
    return startPin;
}

PinItem *WireItem::getEndPin()
{
    if (!endPin) {
        qDebug() << "End pin is not defined!";
        return nullptr;
    }

    return endPin;
}
