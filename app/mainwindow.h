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

    // --- СЛОТЫ ВЕРХНЕЙ ПАНЕЛИ ---
    void runSimulation();
    void zoomIn();
    void zoomOut();
    void fitToScreen();

private:
    Ui::MainWindow *ui;
    CircuitScene *m_scene;
    ComponentLibraryManager componentLibrary;
    std::unique_ptr<Circuit> currentCircuit;

    // --- ФУНКЦИИ И ПЕРЕМЕННЫЕ ВЕРХНЕЙ ПАНЕЛИ ---
    void createActions();
    void createToolbars();

    QAction *actionSave;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCalculate;
    QAction *actionZoomIn;
    QAction *actionZoomOut;
    QAction *actionFit;

    QToolBar *standardToolBar;
    QToolBar *simulationToolBar;
    QToolBar *viewToolBar;
};
#endif