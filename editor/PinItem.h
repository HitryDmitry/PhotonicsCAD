#pragma once
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>

class PinInstance;

class PinItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    PinItem(PinInstance *pin, QGraphicsItem *parent = nullptr);
    PinInstance *getPin() const;

signals:
    void connectionStarted(PinItem *pin);
    void connectionCompleted(PinItem *from, PinItem *to);
    void connectionCancelled();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    PinInstance *pin;
};
