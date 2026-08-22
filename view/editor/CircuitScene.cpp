#include "CircuitScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include "GraphicsComponentItem.h"
#include "PinItem.h"
#include "WireItem.h"

CircuitScene::CircuitScene(QObject *parent, CircuitViewModel *cvm)
    : QGraphicsScene(parent)
    , circuitVM(cvm)
{
    circuitVM->addObserver(this);
}

void CircuitScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    // Шаг сетки
    const int gridSize = 20;

    // Настраиваем кисти для рисования линий
    QPen lightPen(QColor(235, 235, 235), 1, Qt::SolidLine); // Светлая линия
    QPen darkPen(QColor(210, 210, 210), 1, Qt::SolidLine);  // Темная линия (каждая пятая)

    // Вычисляем начало координат для текущего видимого куска экрана
    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QList<QLineF> lines;
    QList<QLineF> darkLines;

    //  Рисуем вертикальные линии
    for (qsizetype x = left; x < rect.right(); x += gridSize) {
        if (int(x) % (gridSize * 5) == 0)
            darkLines.append(QLineF(x, rect.top(), x, rect.bottom()));
        else
            lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    }

    //  Рисуем горизонтальные линии
    for (qsizetype y = top; y < rect.bottom(); y += gridSize) {
        if (int(y) % (gridSize * 5) == 0)
            darkLines.append(QLineF(rect.left(), y, rect.right(), y));
        else
            lines.append(QLineF(rect.left(), y, rect.right(), y));
    }

    //  Выводим линии на экран
    painter->setPen(lightPen);
    painter->drawLines(lines);

    painter->setPen(darkPen);
    painter->drawLines(darkLines);
}

void CircuitScene::onPinClicked(PinItem *pin)
{
    if (circuitVM->checkConnectionStarted()) {
        tryToCompleteConnection(startPin, pin);
    } else {
        circuitVM->changeCircuitState(); // Изменяем состояние схемы на Dragging

        startPin = pin;
        tempWire = new WireItem(startPin);
        tempWire->setZValue(-1); // Помещаем провод позади всех
        addItem(tempWire);
    }
}

void CircuitScene::tryToCompleteConnection(PinItem *from, PinItem *to)
{
    if (!from || !to) {
        throw std::runtime_error(
            "TryToCompleteConnection: can't connect if one of the pinItems is nullptr!");
    }
    if (circuitVM->tryToConnect(from->getPinRef(), to->getPinRef())) {
        tempWire->setEndPin(to);

        from->addWire(tempWire);
        to->addWire(tempWire);

        tempWire = nullptr;
        startPin = nullptr;
    }
}

void CircuitScene::cancelConnection()
{
    if (tempWire) {
        qDebug() << "Connection cancelled, deleting tempWire!";
        removeItem(tempWire);
        delete tempWire;
        tempWire = nullptr;

        circuitVM->changeCircuitState();
    }
}

void CircuitScene::onEscapeButton()
{
    cancelConnection();
}

void CircuitScene::onDeleteButton(QGraphicsItem *item)
{
    if (auto *wireItem = qgraphicsitem_cast<WireItem *>(item)) {
        auto *startPin = wireItem->getStartPin();
        auto *endPin = wireItem->getEndPin();

        if (circuitVM->removeWire(startPin->getPinRef(), endPin->getPinRef())) {
            qDebug() << "Wire removed from the circuit.";
            removeItem(wireItem);
            delete wireItem;
        }

        else {
            throw std::runtime_error("Can't delete wire!");
        }
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
    connect(pinToConnect, SIGNAL(pinClicked(PinItem *)), this, SLOT(onPinClicked(PinItem *)));
}