#include <QGraphicsScene>

class WireItem;
class PinItem;
class PinInstance;

class CircuitScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit CircuitScene(QObject *parent = nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    bool canConnect(PinInstance *a, PinInstance *b);

private:
    WireItem *currentWire = nullptr;
    PinItem *startPin = nullptr;
};
