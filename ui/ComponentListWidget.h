#pragma once
#include <QListWidget>

class ComponentListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit ComponentListWidget(QWidget *parent = nullptr);

protected:
    void startDrag(Qt::DropActions supportedActions) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};
