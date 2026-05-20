#include <QGraphicsScene>
#include <QVector>

class WireItem;
class PinItem;
class PinInstance;

class CircuitScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit CircuitScene(QObject *parent = nullptr);

    bool canConnect(PinInstance *a, PinInstance *b);
    void addItem(QGraphicsItem *item);
    void connectPinToSlots(PinItem *pinToConnect);

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
};
