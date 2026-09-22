#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CircuitViewModel.h"
#include "ComponentLibraryManager.h"

class CircuitScene;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBar;
class QGraphicsItem;
class QProgressBar;
class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class ComponentListWidget;
class GraphicsView;
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public ICircuitObserver
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // Реализация чисто виртуального метода интерфейса наблюдателя
    void onComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def) override;
    void onGraphBuildingCompleted() override;
    void onSimulationCompleted(const std::vector<PinRef> &observationPoints) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void escButtonPressed();
    void deleteButtonPressed(QGraphicsItem *item);

public slots:
    void onComponentDropped(const QString &type, const QPointF &pos);
    void onComponentDoubleClicked(ComponentId id);

    // --- СЛОТЫ ВЕРХНЕЙ ПАНЕЛИ ---
    void runSimulation();
    void zoomIn();
    void zoomOut();
    void fitToScreen();

private:
    void createCentralWidget();
    void createComponentPanel(QHBoxLayout *contentLayout);
    void createGraphicsView(QHBoxLayout *contentLayout);
    void createMenuBar();
    void createStatusBar();

    CircuitScene *m_scene = nullptr;
    ComponentLibraryManager componentLibrary;

    std::unique_ptr<CircuitViewModel> viewModel;

    // --- ФУНКЦИИ И ПЕРЕМЕННЫЕ ВЕРХНЕЙ ПАНЕЛИ ---
    void createActions();
    void createToolbars();

    QAction *actionSave = nullptr;
    QAction *actionUndo = nullptr;
    QAction *actionRedo = nullptr;
    QAction *actionCalculate = nullptr;
    QAction *actionZoomIn = nullptr;
    QAction *actionZoomOut = nullptr;
    QAction *actionFit = nullptr;

    QToolBar *standardToolBar = nullptr;
    QToolBar *simulationToolBar = nullptr;
    QToolBar *viewToolBar = nullptr;

    QVBoxLayout *mainLayout = nullptr;
    ComponentListWidget *componentList = nullptr;
    GraphicsView *graphicsView = nullptr;

    // --- ИНДИКАТОР ВЫПОЛНЕНИЯ ---
    void createProgressBar();
    QWidget *progressContainer = nullptr;
    QHBoxLayout *progressLayout = nullptr;
    QProgressBar *progressBar = nullptr;
    QLabel *progressBarLabel = nullptr;
};
#endif
