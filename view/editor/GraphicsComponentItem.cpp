#include "GraphicsComponentItem.h"
#include "PinInstance.h"
#include "PinItem.h"
#include "WireItem.h"

#include <QDebug>
#include <QPixmap>
#include <qmath.h>
#include <qpainter.h>

GraphicsComponentItem::GraphicsComponentItem(ComponentViewModel *compViewModel,
                                             const ComponentDefinition *def)
    : mComponentVM(compViewModel)
{
    setPixmap(QPixmap(def->iconPath));
    setScale(0.25);

    componentType = def->type;
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);

    createPinItems();
}

GraphicsComponentItem::~GraphicsComponentItem() {}

void GraphicsComponentItem::onPropertyModyfied() {}

QVariant GraphicsComponentItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QVariant newValue = value;

    // SNAP TO GRID
    // Срабатывает только когда пользователь двигает объект по сцене
    if (change == ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();

        // Шаг сетки. Сейчас стоит 20 пикселей.
        int gridSize = 20;

        qreal xV = qRound(newPos.x() / gridSize) * gridSize;
        qreal yV = qRound(newPos.y() / gridSize) * gridSize;

        // Записываем новые координаты
        newValue = QPointF(xV, yV);
    }

    // ОБНОВЛЕНИЕ ПРОВОДОВ
    for (const auto &pinItemIter : std::as_const(mPins)) {
        for (const auto &wireItemIter : pinItemIter->getWireItems()) {
            wireItemIter->updatePath();
        }
    }

    // Возвращаем newValue (которое мы примагнитили к сетке)
    return QGraphicsPixmapItem::itemChange(change, newValue);
}

const QString &GraphicsComponentItem::getComponentType()
{
    return componentType;
}

ComponentId GraphicsComponentItem::getComponentId()
{
    return mComponentVM->getId();
}

QVector<PinItem *> GraphicsComponentItem::getPins()
{
    return mPins;
}

void GraphicsComponentItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    this->hideGreenFrame();
    emit doubleClicked(mComponentVM->getId());
    QGraphicsPixmapItem::mouseDoubleClickEvent(event);
}

void GraphicsComponentItem::createPinItems()
{
    auto &pins = mComponentVM->getInstancePins();
    int count = pins.size();

    for (int i = 0; i < count; i++) {
        auto pinItem = new PinItem(this, mComponentVM, pins.at(i).get()->getPinIdx());

        mPins.push_back(pinItem);

        QPixmap componentPixmap = this->pixmap();
        int pixWidth = componentPixmap.width();
        int pixHeight = componentPixmap.height();

        if (count == 1) {
            pinItem->setPos(pixWidth, pixHeight / 2);
        } else if (count == 2) {
            pinItem->setPos(i * pixWidth, pixHeight / 2);
        } else if (count == 3) {
            if (i == 1) {
                pinItem->setPos(pixWidth / 2, pixHeight);
            } else if (i == 2) {
                pinItem->setPos(pixWidth, pixHeight / 2);
            } else {
                pinItem->setPos(0, pixHeight / 2);
            }
        }
    }
}

// ============================================================
// Frame management
// ============================================================
void GraphicsComponentItem::showGreenFrame()
{
    if (mFrameVisible) {
        return; // Already visible
    }

    prepareGeometryChange(); // Invalidate old bounding rect
    mFrameVisible = true;
    update(); // Schedule repaint
}

void GraphicsComponentItem::hideGreenFrame()
{
    if (!mFrameVisible) {
        return; // Already hidden
    }

    prepareGeometryChange(); // Invalidate old bounding rect
    mFrameVisible = false;
    update(); // Schedule repaint
}

// ============================================================
// Helper: build the frame path (rectangle around the pixmap)
// ============================================================

QPainterPath GraphicsComponentItem::framePath() const
{
    QPainterPath path;

    // Use the pixmap's bounding rect as the base shape
    QRectF rect = QGraphicsPixmapItem::boundingRect();
    if (rect.isEmpty()) {
        return path;
    }

    // Add a small inset so the frame hugs the component
    const qreal inset = 1.0;
    rect = rect.adjusted(inset, inset, -inset, -inset);

    // Rounded rectangle looks nicer for components
    path.addRoundedRect(rect, 4.0, 4.0);

    return path;
}

// ============================================================
// boundingRect: must include the frame area when visible
// ============================================================

QRectF GraphicsComponentItem::boundingRect() const
{
    QRectF base = QGraphicsPixmapItem::boundingRect();

    if (mFrameVisible) {
        qreal margin = FRAME_WIDTH / 2.0 + 5.0; // +5 safety margin
        return base.adjusted(-margin, -margin, margin, margin);
    }

    return base;
}

// ============================================================
// shape: controls the clickable area
// ============================================================

QPainterPath GraphicsComponentItem::shape() const
{
    // Default shape is the pixmap's bounding rect
    QPainterPath basePath;
    basePath.addRect(QGraphicsPixmapItem::boundingRect());

    if (!mFrameVisible) {
        return basePath;
    }

    // When frame is visible, also include the glow area for hit testing
    QPainterPathStroker stroker;
    stroker.setWidth(FRAME_WIDTH);
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);

    QPainterPath frameHitArea = stroker.createStroke(framePath());

    // Union the base shape with the frame hit area
    return basePath.united(frameHitArea);
}

// ============================================================
// paint: draw the pixmap + green glow frame when visible
// ============================================================

void GraphicsComponentItem::paint(QPainter *painter,
                                  const QStyleOptionGraphicsItem *option,
                                  QWidget *widget)
{
    // 1. Draw the component (pixmap) normally
    QGraphicsPixmapItem::paint(painter, option, widget);

    // 2. Draw the green frame if enabled
    if (mFrameVisible) {
        QPainterPath path = framePath();
        if (!path.isEmpty()) {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            // Outer glow (wide, very transparent)
            {
                QPainterPathStroker stroker;
                stroker.setWidth(FRAME_WIDTH);
                stroker.setCapStyle(Qt::RoundCap);
                stroker.setJoinStyle(Qt::RoundJoin);
                QPainterPath outerGlow = stroker.createStroke(path);

                painter->setBrush(QColor(50, 205, 50, 30)); // Lime green, very transparent
                painter->setPen(Qt::NoPen);
                painter->drawPath(outerGlow);
            }

            // Middle glow (medium width, medium transparency)
            {
                QPainterPathStroker stroker;
                stroker.setWidth(14.0);
                stroker.setCapStyle(Qt::RoundCap);
                stroker.setJoinStyle(Qt::RoundJoin);
                QPainterPath midGlow = stroker.createStroke(path);

                painter->setBrush(QColor(50, 205, 50, 80));
                painter->drawPath(midGlow);
            }

            // Inner glow (narrow, more opaque)
            {
                QPainterPathStroker stroker;
                stroker.setWidth(6.0);
                stroker.setCapStyle(Qt::RoundCap);
                stroker.setJoinStyle(Qt::RoundJoin);
                QPainterPath innerGlow = stroker.createStroke(path);

                painter->setBrush(QColor(50, 205, 50, 150));
                painter->drawPath(innerGlow);
            }

            // Core (thin bright green border)
            {
                QPainterPathStroker stroker;
                stroker.setWidth(2.0);
                stroker.setCapStyle(Qt::RoundCap);
                stroker.setJoinStyle(Qt::RoundJoin);
                QPainterPath coreGlow = stroker.createStroke(path);

                painter->setBrush(QColor(34, 139, 34, 220)); // Forest green
                painter->drawPath(coreGlow);
            }

            painter->restore();
        }
    }
}