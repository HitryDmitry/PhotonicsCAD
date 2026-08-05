#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QKeyEvent>
#include <QMetaMethod>
#include "CircuitScene.h"
#include "ComponentDefinition.h"
#include "ComponentInstance.h"
#include "GraphicsView.h"
#include "PropertyEditorDialog.h"

// --- БИБЛИОТЕКИ ДЛЯ ПАНЕЛИ ---
#include <QAction>
#include <QToolBar>
#include <QStyle>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // --- СОЗДАНИЕ ВЕРХНЕЙ ПАНЕЛИ ---
    createActions();
    createToolbars();

    ui->listWidget->setDragEnabled(true);
    ui->listWidget->setDragDropMode(ComponentListWidget::DragOnly);

    //Создание ViewModel, которая внутри себя создаст и будет владеть Circuit
    viewModel = std::make_unique<CircuitViewModel>();

    // Регистрация MainWindow как наблюдателя во ViewModel
    viewModel->addObserver(this);

    // Создание сцены
    m_scene = new CircuitScene(this);

    // Обработка нажатия Escape - удаление временного провода
    connect(this, SIGNAL(escButtonPressed()), m_scene, SLOT(onEscapeButton()));

    // Сигнал на удаление (провода или компонента) - строго по архитектуре
    connect(this,
            SIGNAL(deleteButtonPressed(QGraphicsItem *)),
            m_scene,
            SLOT(onDeleteButton(QGraphicsItem *)));

    // Передаем указатель на схему, которой теперь владеет ViewModel, в сцену-редактор
    m_scene->setCircuit(viewModel->getCircuit());

    // Устанавливаем сцену в GraphicsView из ui
    ui->graphicsView->setScene(m_scene);

    // Загрузка стандартных компонентов
    if (componentLibrary.loadFromJson(":/data/components.json")) {
        qDebug() << "Components successfully loaded.";
    } else {
        qDebug() << "Components haven't been loaded.";
    }

    for (const auto &comp : componentLibrary.getComponents()) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(comp.iconPath), comp.name);
        item->setData(Qt::UserRole, comp.type);
        ui->listWidget->addItem(item);
    }

    // Связь Drag & Drop
    connect(ui->graphicsView,
            SIGNAL(componentDropped(QString, QPointF)),
            this,
            SLOT(onComponentDropped(QString, QPointF)));
}

// Вызывается при падении иконки на сцену
void MainWindow::onComponentDropped(const QString &type, const QPointF &pos)
{
    const ComponentDefinition *def = componentLibrary.getByType(type);

    if (!def) {
        qDebug() << "Can't get ComponentDefinition from componentLibrary by type.";
        return;
    }

    // Вся логика делегируется ViewModel. ViewModel создаст модель компонента
    // через фабрику и автоматически уведомит нас
    viewModel->addComponent(*def, pos.x(), pos.y());
}

// Реализация паттерна "Наблюдатель"
void MainWindow::onComponentAdded(ComponentInstance *instance, const ComponentDefinition *def)
{
    // Отрисовываем графическое представление для добавленного компонента
    auto item = new GraphicsComponentItem(instance, def);

    connect(item,
            SIGNAL(doubleClicked(ComponentInstance *)),
            this,
            SLOT(onComponentDoubleClicked(ComponentInstance *)));

    // Позиционируем элемент на сцене в соответствии со значениями из доменной модели
    item->setPos(instance->mPosition.x, instance->mPosition.y);

    m_scene->addItem(item);
}

void MainWindow::onItemSelected(GraphicsComponentItem *item)
{
    ComponentInstance *instance = item->getInstance();

    if (!instance) {
        qDebug() << "Can't retrieve ComponentInstance from selected GraphicsComponentItem.";
    }

    qDebug() << "Position: " << instance->mPosition.x << ", " << instance->mPosition.y;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        emit escButtonPressed();
    } else if (event->key() == Qt::Key_Delete) {
        // Строго по архитектуре: просто берем элемент и кидаем сигнал в CircuitScene
        QList<QGraphicsItem *> selectedItems = m_scene->selectedItems();
        if (!selectedItems.isEmpty()) {
            QGraphicsItem *item = selectedItems.first();
            emit deleteButtonPressed(item);
        } else {
            qDebug() << "No items selected, can't delete!";
        }
    } else {
        QWidget::keyPressEvent(event);
    }
}

void MainWindow::onComponentDoubleClicked(ComponentInstance *instance)
{
    const ComponentDefinition *def = componentLibrary.getByType(
        QString::fromStdString(instance->mType));

    if (!def)
        return;

    PropertyEditorDialog dialog(instance, def, this);
    dialog.exec();
}

MainWindow::~MainWindow()
{
    // Корректная отписка от наблюдателя в деструкторе (как в Части 3 видеоуроков!)
    if (viewModel) {
        viewModel->removeObserver(this);
    }
    delete ui;
}

// СОЗДАНИЕ ИНТЕРФЕЙСА ПАНЕЛИ ИНСТРУМЕНТОВ

void MainWindow::createActions()
{
    actionSave = new QAction(style()->standardIcon(QStyle::SP_DriveFDIcon), tr("Save"), this);
    actionSave->setShortcut(QKeySequence::Save);

    actionUndo = new QAction(style()->standardIcon(QStyle::SP_ArrowBack), tr("Undo"), this);
    actionUndo->setShortcut(QKeySequence::Undo);

    actionRedo = new QAction(style()->standardIcon(QStyle::SP_ArrowForward), tr("Redo"), this);
    actionRedo->setShortcut(QKeySequence::Redo);

    actionCalculate = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Calculate (Run)"), this);
    actionCalculate->setShortcut(Qt::Key_F5);
    connect(actionCalculate, &QAction::triggered, this, &MainWindow::runSimulation);

    actionZoomIn = new QAction(tr("Zoom In (+)"), this);
    connect(actionZoomIn, &QAction::triggered, this, &MainWindow::zoomIn);

    actionZoomOut = new QAction(tr("Zoom Out (-)"), this);
    connect(actionZoomOut, &QAction::triggered, this, &MainWindow::zoomOut);

    actionFit = new QAction(style()->standardIcon(QStyle::SP_DesktopIcon), tr("Fit to Window"), this);
    connect(actionFit, &QAction::triggered, this, &MainWindow::fitToScreen);
}

void MainWindow::createToolbars()
{
    standardToolBar = addToolBar(tr("Standard"));
    standardToolBar->addAction(actionSave);
    standardToolBar->addAction(actionUndo);
    standardToolBar->addAction(actionRedo);

    simulationToolBar = addToolBar(tr("Simulation"));
    simulationToolBar->addAction(actionCalculate);

    viewToolBar = addToolBar(tr("View"));
    viewToolBar->addAction(actionZoomIn);
    viewToolBar->addAction(actionZoomOut);
    viewToolBar->addAction(actionFit);
}

// ЛОГИКА РАБОТЫ КНОПОК ПАНЕЛИ

void MainWindow::runSimulation() { qDebug() << "Запуск расчета схемы..."; }

void MainWindow::zoomIn() { ui->graphicsView->scale(1.25, 1.25); }

void MainWindow::zoomOut() { ui->graphicsView->scale(0.8, 0.8); }

void MainWindow::fitToScreen()
{
    if(m_scene) {
        ui->graphicsView->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}