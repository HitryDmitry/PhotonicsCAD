#pragma once
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>

class PinInstance;

class PinItem : public QGraphicsEllipseItem
{
public:
    PinItem(PinInstance *pin, QGraphicsItem *parent = nullptr);

    PinInstance *getPin() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    PinInstance *pin;
};
