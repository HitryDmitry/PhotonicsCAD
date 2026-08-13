#include "GraphicsComponentItem.h"
#include <QDebug>
#include <QPixmap>
#include <qmath.h>
#include <map>
#include <string>
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

    createPinItems(def);
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
    for (const auto &pinItemIter : std::as_const(pins)) {
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

QVector<PinItem *> GraphicsComponentItem::getPins()
{
    return pins;
}

void GraphicsComponentItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit doubleClicked(mComponentVM);
    QGraphicsPixmapItem::mouseDoubleClickEvent(event);
}

void GraphicsComponentItem::createPinItems(const ComponentDefinition *def)
{
    int count = def->pins.size();

    for (int i = 0; i < count; i++) {
        // Ссылка на словарь, описывающий пин: def->pins.at(i)
        // Возможно это понадобится при определении положения пинов в зависимости от
        // параметров пина
        const auto &currentPin = def->pins.at(i);

        // Конвертируем QMap<QString, QVariant> в std::map<std::string, std::string>
        std::map<std::string, std::string> stdPinDef;
        for (const auto &[key, value] : currentPin.asKeyValueRange()) {
            stdPinDef[key.toStdString()] = value.toString().toStdString();
        }

        auto pinItem = new PinItem(mComponentVM, this);

        pins.push_back(pinItem);

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