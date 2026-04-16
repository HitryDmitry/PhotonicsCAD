#include "GraphicsView.h"
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QGraphicsScene>
#include <QMimeData>

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    setAcceptDrops(true);
}

void GraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText())
        event->acceptProposedAction();
}

void GraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    // if (!event->mimeData()->hasText()) {
    //     qDebug() << "Drop event - no text in MIME data, ignoring";
    //     event->ignore();
    //     return;
    // }

    // QString componentType = event->mimeData()->text();

    // QPoint viewPos = event->position().toPoint();
    // QPointF scenePos = mapToScene(viewPos);

    // qDebug() << "MOVE: type=" << componentType << "; viewPos=" << viewPos
    //          << "; scenePos=" << scenePos;

    // emit componentMoved(componentType, scenePos);

    event->acceptProposedAction();
}

void GraphicsView::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasText()) {
        qDebug() << "Drop event - no text in MIME data, ignoring";
        event->ignore();
        return;
    }

    QString componentType = event->mimeData()->text();

    QPoint viewPos = event->position().toPoint();
    QPointF scenePos = mapToScene(viewPos);

    qDebug() << "DROP: type=" << componentType << "; viewPos=" << viewPos
             << "; scenePos=" << scenePos;

    emit componentDropped(componentType, scenePos);

    update();
}
