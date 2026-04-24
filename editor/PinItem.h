#pragma once
#include <QGraphicsEllipseItem>

class PinInstance;

class PinItem : public QGraphicsEllipseItem
{
public:
    PinItem(PinInstance *pin, QGraphicsItem *parent = nullptr);

    PinInstance *getPin() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    PinInstance *pin;
};
