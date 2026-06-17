#include "WireItem.h"
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include "PinItem.h"

WireItem::WireItem(PinItem *startPin)
    : startPin(startPin)
{
    setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setFlag(ItemIsSelectable);

    // Initialize with a minimal valid path
    QPainterPath initPath;
    initPath.moveTo(startPin->scenePos());
    initPath.lineTo(startPin->scenePos());
    setPath(initPath);
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

QRectF WireItem::boundingRect() const
{
    // Include the path + glow area + some margin for safety
    qreal margin = GLOW_WIDTH / 2.0 + 5.0; // +5 for safety margin
    return path().boundingRect().adjusted(-margin, -margin, margin, margin);
}

QPainterPath WireItem::shape() const
{
    // Get the original path (the actual wire)
    QPainterPath originalPath = path();

    // Create a stroker that adds a margin around the wire
    QPainterPathStroker stroker;
    stroker.setWidth(pen().widthF() + 8.0); // pen width + 4px on each side
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);

    // Return the thickened path used for hit testing
    return stroker.createStroke(originalPath);
}

void WireItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Create a copy of the option without the selection state
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected; // Remove selection flag

    // Draw the wire without selection rectangle
    QGraphicsPathItem::paint(painter, &myOption, widget);

    if (option->state & QStyle::State_Selected) {
        QPainterPath originalPath = path();
        if (!originalPath.isEmpty()) {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            // Create a glow effect with multiple layers for smooth perpendicular fade
            // Outer glow (wide, very transparent)
            QPainterPathStroker outerStroker;
            outerStroker.setWidth(GLOW_WIDTH);
            outerStroker.setCapStyle(Qt::RoundCap);
            outerStroker.setJoinStyle(Qt::RoundJoin);
            QPainterPath outerGlow = outerStroker.createStroke(originalPath);

            painter->setBrush(QColor(135, 206, 235, 30));
            painter->setPen(Qt::NoPen);
            painter->drawPath(outerGlow);

            // Middle glow (medium width, medium transparency)
            QPainterPathStroker midStroker;
            midStroker.setWidth(14.0);
            midStroker.setCapStyle(Qt::RoundCap);
            midStroker.setJoinStyle(Qt::RoundJoin);
            QPainterPath midGlow = midStroker.createStroke(originalPath);

            painter->setBrush(QColor(135, 206, 235, 80));
            painter->drawPath(midGlow);

            // Inner glow (narrow, more opaque)
            QPainterPathStroker innerStroker;
            innerStroker.setWidth(6.0);
            innerStroker.setCapStyle(Qt::RoundCap);
            innerStroker.setJoinStyle(Qt::RoundJoin);
            QPainterPath innerGlow = innerStroker.createStroke(originalPath);

            painter->setBrush(QColor(135, 206, 235, 150));
            painter->drawPath(innerGlow);

            // Core (the wire itself, slightly blue)
            QPainterPathStroker coreStroker;
            coreStroker.setWidth(2.0);
            coreStroker.setCapStyle(Qt::RoundCap);
            coreStroker.setJoinStyle(Qt::RoundJoin);
            QPainterPath coreGlow = coreStroker.createStroke(originalPath);

            painter->setBrush(QColor(70, 130, 180, 200)); // Steel blue
            painter->drawPath(coreGlow);

            painter->restore();
        }
    }
}