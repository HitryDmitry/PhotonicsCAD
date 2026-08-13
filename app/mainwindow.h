#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CircuitViewModel.h"
#include "ComponentLibraryManager.h"
#include "GraphicsComponentItem.h"

class CircuitScene;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBar;
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public ICircuitObserver
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // Реализация чисто виртуального метода интерфейса наблюдателя
    void onComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def) override;

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

    std::unique_ptr<CircuitViewModel> viewModel;

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