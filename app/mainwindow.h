#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ComponentLibraryManager.h"
#include "GraphicsComponentItem.h"

class CircuitScene;
class Circuit;


QT_BEGIN_NAMESPACE
class QAction;
class QToolBar;
class QActionGroup;
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

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void escButtonPressed();
    void deleteButtonPressed(QGraphicsItem *item);

public slots:
    void onComponentDropped(const QString &type, const QPointF &pos);
    void onComponentDoubleClicked(ComponentInstance *instance);

    // верхняя панель
    void runSimulation();
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void setModeSelect();
    void setModeConnect();
    void onDeleteActionTriggered(); // Умное удаление элементов

private:
    Ui::MainWindow *ui;
    CircuitScene *m_scene;
    ComponentLibraryManager componentLibrary;
    std::unique_ptr<Circuit> currentCircuit;

    // функции и переменные панели
    void createActions();
    void createToolbars();

    QAction *actionSave;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionDelete;
    QAction *actionCalculate;
    QAction *actionZoomIn;
    QAction *actionZoomOut;
    QAction *actionFit;
    QAction *actionSelect;
    QAction *actionConnect;

    QActionGroup *modeActionGroup;

    QToolBar *standardToolBar;
    QToolBar *simulationToolBar;
    QToolBar *viewToolBar;
    QToolBar *layoutToolBar;
};
#endif