#include <QGraphicsScene>
#include <QVector>
#include "Wire.h"

class WireItem;
class PinItem;
class PinInstance;
class Circuit;

class CircuitScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit CircuitScene(QObject *parent = nullptr);

    bool canConnect(PinInstance *a, PinInstance *b);
    void addItem(QGraphicsItem *item);
    void connectPinToSlots(PinItem *pinToConnect);
    void setCircuit(Circuit *circuit);

public slots:
    void onConnectionStarted(PinItem *pin);
    void onConnectionCompleted(PinItem *from, PinItem *to);
    void onCoonnectionCancelled();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    WireItem *tempWire = nullptr;
    PinItem *startPin = nullptr;

    Circuit *circuit = nullptr;
};
