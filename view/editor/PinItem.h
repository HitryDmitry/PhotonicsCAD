#pragma once
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QSet>

#include "PinRef.h"

class ComponentViewModel;
class WireItem;

class PinItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    PinItem(ComponentViewModel *cvm, QGraphicsItem *parent = nullptr);
    void addWire(WireItem *);
    const QSet<WireItem *> &getWireItems();

    ComponentId getComponentId();
    PinIndex getPinIdx();
    PinRef getPinRef();

signals:
    void pinClicked(PinItem *pin);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    ComponentViewModel *mCompVM;
    QSet<WireItem *> wireItems;
    PinIndex mIdx;
};
