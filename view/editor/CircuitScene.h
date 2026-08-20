#ifndef CIRCUITSCENE_H  // Защита от двойного включения (на всякий случай)
#define CIRCUITSCENE_H

#include <QGraphicsScene>

#include "CircuitViewModel.h"

class WireItem;
class PinItem;
class Circuit;

class CircuitScene : public QGraphicsScene, public ICircuitObserver
{
    Q_OBJECT

public:
    explicit CircuitScene(QObject *parent = nullptr, CircuitViewModel *cvm = nullptr);

    void addItem(QGraphicsItem *item);
    void connectPinToSlots(PinItem *pinToConnect);
    void setCircuit(Circuit *circuit);

    ~CircuitScene() override {};
    void onComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def) override {};

public slots:
    void onConnectionStarted(PinItem *pin);
    void onConnectionCompleted(PinItem *from, PinItem *to);
    void onConnectionCancelled();
    void onEscapeButton();
    void onDeleteButton(QGraphicsItem *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    // -отрисовка сетки
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    WireItem *tempWire = nullptr;
    PinItem *startPin = nullptr;

    CircuitViewModel *circuitVM;
};

#endif