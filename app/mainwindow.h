#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QMainWindow>
#include "ComponentLibraryManager.h"
#include "GraphicsComponentItem.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void onItemSelected(GraphicsComponentItem *item);

public slots:
    void onComponentDropped(const QString &type, const QPointF &pos);
    void onComponentDoubleClicked(ComponentInstance *instance);
    // void onComponentMoved(const QString &type, const QPointF &newPos);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *m_scene;
    ComponentLibraryManager componentLibrary;
};
#endif // MAINWINDOW_H
