#include "CircuitScene.h"
#include <QGraphicsSceneMouseEvent>
#include "Circuit.h"
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
        onConnectionCancelled();
    } else {
        auto fromPin = from->getPin();
        auto toPin = to->getPin();

        WireKey key(fromPin, toPin);

        if (canConnect(fromPin, toPin) && !circuit->wireIndex.contains(key)) {
            tempWire->setEndPin(to);

            auto wire = std::make_unique<Wire>(fromPin, toPin);
            auto wirePtr = wire.get();

            from->addWire(tempWire);
            fromPin->addWirePtr(wirePtr);

            to->addWire(tempWire);
            toPin->addWirePtr(wirePtr);

            circuit->wireIndex.insert(key);
            circuit->wires.push_back(std::move(wire));

            tempWire = nullptr;
            startPin = nullptr;
        } else {
            qDebug() << "Can't connect!!";
        }
    }
}

void CircuitScene::onConnectionCancelled()
{
    if (tempWire) {
        qDebug() << "Connection cancelled, deleting tempWire!";
        removeItem(tempWire);
        delete tempWire;
        tempWire = nullptr;
    }
}

void CircuitScene::onEscapeButton()
{
    onConnectionCancelled();
}

void CircuitScene::onDeleteButton(QGraphicsItem *item)
{
    if (auto *wireItem = qgraphicsitem_cast<WireItem *>(item)) {
        qDebug() << "Deleting selected wire!";

        WireKey key(wireItem->getStartPin()->getPin(), wireItem->getEndPin()->getPin());
        if (circuit->wireIndex.contains(key)) {
            circuit->wireIndex.erase(key);
            auto &wires = circuit->wires;
            for (auto it = wires.begin(); it != wires.end();) {
                auto &wirePtr = *it;
                bool found = (wirePtr->from == wireItem->getStartPin()->getPin()
                              && wirePtr->to == wireItem->getEndPin()->getPin())
                             || (wirePtr->to == wireItem->getStartPin()->getPin()
                                 && wirePtr->from == wireItem->getEndPin()->getPin());
                if (wirePtr && found) {
                    it = wires.erase(it); // erase возвращает следующий валидный итератор
                } else {
                    ++it;
                }
            }
        }

        removeItem(wireItem);
        // TODO: Удалить этот провод из всех контейнеров
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
    connect(pinToConnect, SIGNAL(connectionCancelled()), this, SLOT(onConnectionCancelled()));
    // connect(pinToConnect, , this, updateTempWire());
}

void CircuitScene::setCircuit(Circuit *newCircuit)
{
    circuit = newCircuit;
}
