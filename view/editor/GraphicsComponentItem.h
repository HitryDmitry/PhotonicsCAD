#pragma once
#include <QGraphicsPixmapItem>

#include "ComponentDefinition.h"
#include "ComponentViewModel.h"

class PinItem;

class GraphicsComponentItem : public QObject, public QGraphicsPixmapItem, public IComponentObserver
{
    Q_OBJECT
public:
    GraphicsComponentItem(ComponentViewModel *compViewModel, const ComponentDefinition *def);

    ~GraphicsComponentItem() override;
    void onPropertyModyfied() override;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void createPinItems();
    QVector<PinItem *> getPins();

    const QString &getComponentType();

signals:
    void doubleClicked(ComponentId id);

private:
    QString componentType;
    QVector<PinItem *> mPins;
    ComponentViewModel *mComponentVM;
};
