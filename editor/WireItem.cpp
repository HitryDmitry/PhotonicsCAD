#include "WireItem.h"
#include <QPainterPath>
#include "PinItem.h"
#include <qpen.h>

WireItem::WireItem(PinItem *startPin)
    : startPin(startPin)
{
    setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setFlag(ItemIsSelectable);
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
    prepareGeometryChange();

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

// QRectF WireItem::boundingRect() const
// {
//     return shape().boundingRect();
// }

// QPainterPath WireItem::shape() const
// {
//     // Get the original path (the actual wire)
//     QPainterPath originalPath = path();

//     // Create a stroker that adds a margin around the wire
//     QPainterPathStroker stroker;
//     stroker.setWidth(pen().widthF() + 4.0); // pen width + 2px on each side
//     stroker.setCapStyle(Qt::RoundCap);
//     stroker.setJoinStyle(Qt::RoundJoin);

//     // Return the thickened path used for hit testing
//     return stroker.createStroke(originalPath);
// }

QPainterPath WireItem::shape() const
{
    QPainterPath original = path();
    // qDebug() << "shape() called, original path element count:" << original.elementCount();
    if (original.isEmpty()) {
        qDebug() << "WARNING: path is empty!";
    }
    QPainterPathStroker stroker;
    stroker.setWidth(pen().widthF() + 4.0);
    QPainterPath stroked = stroker.createStroke(original);
    // qDebug() << "stroked bounding rect:" << stroked.boundingRect();
    return stroked;
}

QRectF WireItem::boundingRect() const
{
    QRectF rect = shape().boundingRect();
    // qDebug() << "boundingRect() returns:" << rect;
    return rect;
}