#include "GraphicsComponentItem.h"
#include <QDebug>
#include <QPixmap>
#include <QGraphicsScene> // Нужно для проверки пересечений
#include <qmath.h>

#include "PinInstance.h"
#include "PinItem.h"
#include "WireItem.h"

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

// Переопределяем хитбокс компонента (для коллизий)
QRectF GraphicsComponentItem::boundingRect() const
{
    QRectF originalRect = QGraphicsPixmapItem::boundingRect();
    double padding = 20.0; // Отступ, чтобы блоки не слипались вплотную
    return originalRect.adjusted(-padding, -padding, padding, padding);
}

// =========================================================
// Перехватываем перемещение (Сетка + Защита от наложений)
// =========================================================
QVariant GraphicsComponentItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        QPointF proposedPos = value.toPointF();

        // 1. ПРИВЯЗКА К СЕТКЕ (SNAP TO GRID)
        int gridSize = 20;
        qreal xV = qRound(proposedPos.x() / gridSize) * gridSize;
        qreal yV = qRound(proposedPos.y() / gridSize) * gridSize;
        QPointF snappedPos(xV, yV);

        // 2. ЗАЩИТА ОТ НАЛОЖЕНИЙ
        QPolygonF proposedShape = mapToScene(boundingRect());
        proposedShape.translate(snappedPos - pos()); // Сдвигаем хитбокс на новую позицию

        // Получаем все элементы сцены, которые пересекаются с новой позицией
        QList<QGraphicsItem*> itemsInArea = scene()->items(proposedShape, Qt::IntersectsItemBoundingRect);

        for (QGraphicsItem* item : itemsInArea) {
            if (item == this) continue;

            // Если наткнулись на другой компонент (игнорируем провода и пины)
            if (dynamic_cast<GraphicsComponentItem*>(item)) {
                return pos(); // Отменяем перемещение (возвращаем старую позицию)
            }
        }

        // 3. ОБНОВЛЕНИЕ ПРОВОДОВ
        for (const auto &pinItemIter : std::as_const(mPins)) {
            for (const auto &wireItemIter : pinItemIter->getWireItems()) {
                wireItemIter->updatePath();
            }
        }

        return snappedPos;
    }

    return QGraphicsPixmapItem::itemChange(change, value);
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