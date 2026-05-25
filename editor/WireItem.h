#pragma once
#include <QGraphicsPathItem>

class PinItem;

class WireItem : public QGraphicsPathItem
{
public:
    WireItem(PinItem *startPin);

    void setEndPoint(const QPointF &pos);
    void setEndPin(PinItem *pin);
    void updatePath();

    PinItem *getStartPin();
    PinItem *getEndPin();

private:
    PinItem *startPin;
    PinItem *endPin = nullptr;
    QPointF tempEnd;

    bool plotNormalToX = false;
};
