#include "CircuitScene.h"
#include <QGraphicsSceneMouseEvent>
#include "PinInstance.h"
#include "PinItem.h"
#include "WireItem.h"

CircuitScene::CircuitScene(QObject *parent)
    : QGraphicsScene(parent)
{}

void CircuitScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    auto item = itemAt(event->scenePos(), QTransform());

    auto pin = dynamic_cast<PinItem *>(item);

    if (pin) {
        startPin = pin;

        currentWire = new WireItem(pin);
        addItem(currentWire);
        return;
    }

    QGraphicsScene::mousePressEvent(event);
}

void CircuitScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (currentWire) {
        currentWire->setEndPoint(event->scenePos());
        return;
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void CircuitScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!currentWire)
        return;

    auto item = itemAt(event->scenePos(), QTransform());
    auto endPin = dynamic_cast<PinItem *>(item);

    if (endPin && endPin != startPin) {
        // TODO: проверка совместимости
        currentWire->setEndPin(endPin);

        // здесь создать Wire (domain)
    } else {
        removeItem(currentWire);
        delete currentWire;
    }

    currentWire = nullptr;
    startPin = nullptr;
}

bool CircuitScene::canConnect(PinInstance *a, PinInstance *b)
{
    if (a->signalType != b->signalType)
        return false;

    if (a->direction == b->direction)
        return false;

    return true;
}
