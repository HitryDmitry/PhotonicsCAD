#include "CircuitScene.h"
#include <QGraphicsSceneMouseEvent>
#include "GraphicsComponentItem.h"
#include "PinItem.h"
#include "WireItem.h"

CircuitScene::CircuitScene(QObject *parent)
    : QGraphicsScene(parent)
{}

void CircuitScene::onConnectionStarted(PinItem *pin)
{
    if (tempWire) {
        onConnectionCompleted(startPin, pin);
    } else {
        startPin = pin;
        tempWire = new WireItem(startPin);
        tempWire->setZValue(-1); // Помещаем провод позади всех
        addItem(tempWire);
    }
}

void CircuitScene::onConnectionCompleted(PinItem *from, PinItem *to)
{
    if (from == to) {
        onCoonnectionCancelled();
    } else {
        auto wire = getWireFromPinItemPtrs(from, to);
        if (canConnect(from->getPin(), to->getPin()) && !wireExists(wire)) {
            tempWire->setEndPin(to);

            from->addWire(tempWire);
            from->getPin()->addWirePtr(&wire);

            to->addWire(tempWire);
            to->getPin()->addWirePtr(&wire);

            tempWire = nullptr;
            startPin = nullptr;
        } else {
            qDebug() << "Can't connect!!";
        }
    }
}

void CircuitScene::onCoonnectionCancelled()
{
    if (tempWire) {
        qDebug() << "Connection cancelled, deleting tempWire!";
        removeItem(tempWire);
        delete tempWire;
        tempWire = nullptr;
    }
}

void CircuitScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
}

void CircuitScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (tempWire && startPin) {
        tempWire->setEndPoint(event->scenePos());
    }
    QGraphicsScene::mouseMoveEvent(event);
}

bool CircuitScene::canConnect(PinInstance *a, PinInstance *b)
{
    if (a->signalType != b->signalType)
        return false;

    if (a->direction == b->direction)
        return false;

    if (a->component == b->component)
        return false;

    return true;
}

void CircuitScene::addItem(QGraphicsItem *item)
{
    QGraphicsScene::addItem(item);

    if (auto *comp = qgraphicsitem_cast<GraphicsComponentItem *>(item)) {
        for (auto *pin : comp->getPins()) {
            connectPinToSlots(pin);
        }
    }
}

void CircuitScene::connectPinToSlots(PinItem *pinToConnect)
{
    connect(pinToConnect,
            SIGNAL(connectionStarted(PinItem *)),
            this,
            SLOT(onConnectionStarted(PinItem *)));
    connect(pinToConnect,
            SIGNAL(connectionCompleted(PinItem *, PinItem *)),
            this,
            SLOT(onConnectionCompleted(PinItem *, PinItem *)));
    connect(pinToConnect, SIGNAL(connectionCancelled()), this, SLOT(onCoonnectionCancelled()));
    // connect(pinToConnect, , this, updateTempWire());
}

Wire CircuitScene::getWireFromPinItemPtrs(PinItem *startPin, PinItem *endPin)
{
    if (!endPin) {
        qDebug() << "Can't make wire from wire item, end pin is not defined; return nullptr.";
        return nullptr;
    }
    return Wire(startPin->getPin(), endPin->getPin());
}

bool CircuitScene::wireExists(Wire wire)
{
    return circuitWires.contains(wire);
}

void CircuitScene::setCircuit(Circuit *circuit)
{
    circuit = circuit;
}
