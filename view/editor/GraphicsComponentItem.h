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
    ComponentId getComponentId();

    // Frame management
    void showGreenFrame();
    void hideGreenFrame();
    bool isFrameVisible() const { return mFrameVisible; }

    // Overrides for custom painting
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

signals:
    void doubleClicked(ComponentId id);

private:
    QString componentType;
    QVector<PinItem *> mPins;
    ComponentViewModel *mComponentVM;

    // Helper to compute the frame path
    QPainterPath framePath() const;

    // Frame state
    bool mFrameVisible = false;
    static constexpr qreal FRAME_WIDTH = 24.0; // Maximum glow width
};
