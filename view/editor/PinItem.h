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
    PinItem(QGraphicsItem *parent, ComponentViewModel *cvm, PinIndex idx);
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
