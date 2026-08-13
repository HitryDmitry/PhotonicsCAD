#pragma once
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QSet>

class ComponentViewModel;
class WireItem;

class PinItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    PinItem(ComponentViewModel *cvm, QGraphicsItem *parent = nullptr);
    void addWire(WireItem *);
    const QSet<WireItem *> &getWireItems();

signals:
    void connectionStarted(PinItem *pin);
    void connectionCompleted(PinItem *from, PinItem *to);
    void connectionCancelled();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    ComponentViewModel *mCompVM;
    QSet<WireItem *> wireItems;
};
