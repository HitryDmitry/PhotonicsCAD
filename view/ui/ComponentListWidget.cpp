#include "ComponentListWidget.h"
#include <QDebug>
#include <QDrag>
#include <QMimeData>

ComponentListWidget::ComponentListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void ComponentListWidget::startDrag(Qt::DropActions)
{
    qDebug() << "Drag action started";

    QListWidgetItem *item = currentItem();
    if (!item) {
        qDebug() << "No QListWidgetItem, can't start dragging!!!";
    }

    QString componentType = item->data(Qt::UserRole).toString();

    QMimeData *mimeData = new QMimeData;
    mimeData->setText(componentType);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(item->icon().pixmap(48, 48));

    drag->exec(Qt::CopyAction);
}

void ComponentListWidget::mouseMoveEvent(QMouseEvent *event)
{
    // qDebug() << "mouse move";
    QListWidget::mouseMoveEvent(event);
}
