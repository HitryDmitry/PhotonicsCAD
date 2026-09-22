#include "mainwindow.h"

#include "CircuitScene.h"
#include "ComponentListWidget.h"
#include "ComponentDefinition.h"
#include "ComponentInstance.h"
#include "ComponentViewModel.h"
#include "GraphicsComponentItem.h"
#include "GraphicsView.h"
#include "PropertyEditorDialog.h"

// --- БИБЛИОТЕКИ ДЛЯ ПАНЕЛИ ---
#include <QAction>
#include <QStyle>
#include <QToolBar>

// --- ДРУГИЕ БИБЛИОТЕКИ Qt ---
#include <QDebug>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMetaMethod>
#include <QProgressBar>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(888, 477);
    setMouseTracking(true);
    setWindowTitle(tr("MainWindow"));

    createCentralWidget();
    createMenuBar();
    createStatusBar();

    // --- СОЗДАНИЕ ВЕРХНЕЙ ПАНЕЛИ ---
    createActions();
    createToolbars();

    // --- СОЗДАНИЕ ИНДИКАТОРА ВЫПОЛНЕНИЯ ---
    createProgressBar();

    componentList->setDragEnabled(true);
    componentList->setDragDropMode(ComponentListWidget::DragOnly);

    //Создание ViewModel, которая внутри себя создаст и будет владеть Circuit
    viewModel = std::make_unique<CircuitViewModel>();

    // Регистрация MainWindow как наблюдателя во ViewModel
    viewModel->addObserver(this);

    // Сырой указатель на ViewModel
    auto viewModelPtr = viewModel.get();

    // Создание сцены
    m_scene = new CircuitScene(this, viewModelPtr);

    // Обработка нажатия Escape - удаление временного провода
    connect(this, SIGNAL(escButtonPressed()), m_scene, SLOT(onEscapeButton()));

    // Сигнал на удаление (провода или компонента) - строго по архитектуре
    connect(this,
            SIGNAL(deleteButtonPressed(QGraphicsItem *)),
            m_scene,
            SLOT(onDeleteButton(QGraphicsItem *)));

    graphicsView->setScene(m_scene);

    // Загрузка стандартных компонентов
    if (componentLibrary.loadFromJson(":/data/components.json")) {
        qDebug() << "Components successfully loaded.";
    } else {
        qDebug() << "Components haven't been loaded.";
    }

    for (const auto &comp : componentLibrary.getComponents()) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(comp.iconPath), comp.name);
        item->setData(Qt::UserRole, comp.type);
        componentList->addItem(item);
    }

    // Связь Drag & Drop
    connect(graphicsView,
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
void MainWindow::onComponentAdded(ComponentViewModel *cvm, const ComponentDefinition *def)
{
    // Отрисовываем графическое представление для добавленного компонента
    auto item = new GraphicsComponentItem(cvm, def);

    connect(item,
            SIGNAL(doubleClicked(ComponentId)),
            this,
            SLOT(onComponentDoubleClicked(ComponentId)));

    // Позиционируем элемент на сцене в соответствии со значениями из доменной модели
    item->setPos(cvm->getX(), cvm->getY());

    m_scene->addItem(item);
}

void MainWindow::onGraphBuildingCompleted()
{
    progressBarLabel->setText(tr("Симуляция:"));
    progressBar->setValue(30);
}

void MainWindow::onSimulationCompleted(const std::vector<PinRef> &observationPoints)
{
    progressBarLabel->setText(tr("Симуляция завершена."));
    progressBar->setValue(100);

    actionCalculate->setEnabled(true);
    actionCalculate->setText(tr("Calculate (Run)"));
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

void MainWindow::onComponentDoubleClicked(ComponentId id)
{
    const ComponentDefinition *def = componentLibrary.getByType(
        QString::fromStdString(viewModel->getComponent(id)->getType()));

    if (!def)
        return;

    auto cvm = viewModel->getComponentVM(id);

    // PropertyEditorDialog dialog(cvm, def, this);
    // dialog.exec();

    // 1. Создаем диалог в куче (heap)
    PropertyEditorDialog *dialog = new PropertyEditorDialog(cvm, def, this);

    // 2. Указываем Qt автоматически удалить объект из памяти, когда окно закроется
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    // 3. Делаем его модальным, но без блокировки потока через exec()
    dialog->setWindowModality(Qt::WindowModal);

    // 4. Показываем окно
    dialog->show();
}

MainWindow::~MainWindow()
{
    // Корректная отписка от наблюдателя в деструкторе (как в Части 3 видеоуроков!)
    if (viewModel) {
        viewModel->removeObserver(this);
    }
}

void MainWindow::createCentralWidget()
{
    auto *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralwidget");
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setObjectName("verticalLayout");

    auto *scrollArea = new QScrollArea(centralWidget);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidgetResizable(true);

    auto *scrollContents = new QWidget(scrollArea);
    scrollContents->setObjectName("scrollAreaWidgetContents");
    auto *contentLayout = new QHBoxLayout(scrollContents);
    contentLayout->setObjectName("contentLayout");

    createComponentPanel(contentLayout);
    createGraphicsView(contentLayout);

    scrollArea->setWidget(scrollContents);
    mainLayout->addWidget(scrollArea);
}

void MainWindow::createComponentPanel(QHBoxLayout *contentLayout)
{
    auto *panelLayout = new QVBoxLayout;
    panelLayout->setSizeConstraint(QLayout::SetMaximumSize);

    auto *filterEdit = new QLineEdit(this);
    filterEdit->setObjectName("lineEdit");
    filterEdit->setMaximumWidth(250);
    panelLayout->addWidget(filterEdit);

    componentList = new ComponentListWidget(this);
    componentList->setObjectName("listWidget");
    componentList->setMaximumWidth(250);
    componentList->setMouseTracking(true);
    componentList->setDragEnabled(true);
    componentList->setDragDropMode(QAbstractItemView::NoDragDrop);
    componentList->setDefaultDropAction(Qt::MoveAction);
    componentList->setIconSize(QSize(110, 80));
    componentList->setMovement(QListView::Static);
    componentList->setSpacing(10);
    componentList->setGridSize(QSize(110, 90));
    componentList->setViewMode(QListView::IconMode);
    componentList->setItemAlignment(Qt::AlignLeading);
    panelLayout->addWidget(componentList);

    auto *stackedWidget = new QStackedWidget(this);
    stackedWidget->setObjectName("stackedWidget");
    stackedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    stackedWidget->setMaximumSize(250, 300);
    stackedWidget->addWidget(new QWidget(stackedWidget));
    stackedWidget->addWidget(new QWidget(stackedWidget));
    panelLayout->addWidget(stackedWidget);

    contentLayout->addLayout(panelLayout);
}

void MainWindow::createGraphicsView(QHBoxLayout *contentLayout)
{
    graphicsView = new GraphicsView(this);
    graphicsView->setObjectName("graphicsView");
    graphicsView->setMouseTracking(true);
    contentLayout->addWidget(graphicsView);
}

void MainWindow::createMenuBar()
{
    auto *menuBar = new QMenuBar(this);
    menuBar->setObjectName("menubar");
    setMenuBar(menuBar);
}

void MainWindow::createStatusBar()
{
    auto *mainStatusBar = new QStatusBar(this);
    mainStatusBar->setObjectName("statusbar");
    setStatusBar(mainStatusBar);
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

void MainWindow::createProgressBar()
{
    progressContainer = new QWidget(this);
    progressLayout = new QHBoxLayout(progressContainer);
    progressLayout->setContentsMargins(0, 0, 0, 0);

    progressBar = new QProgressBar(progressContainer);
    progressBar->setRange(0, 100);

    progressBarLabel = new QLabel(tr("Выполнение:"), progressContainer);

    progressLayout->addWidget(progressBarLabel);
    progressLayout->addWidget(progressBar);

    mainLayout->addWidget(progressContainer);
}

// ЛОГИКА РАБОТЫ КНОПОК ПАНЕЛИ
void MainWindow::runSimulation()
{
    qDebug() << "Запуск расчета схемы...";
    actionCalculate->setEnabled(false);
    actionCalculate->setText(tr("Calculating..."));

    viewModel->startSimulation();
}

void MainWindow::zoomIn() { graphicsView->scale(1.25, 1.25); }

void MainWindow::zoomOut() { graphicsView->scale(0.8, 0.8); }

void MainWindow::fitToScreen()
{
    if(m_scene) {
        graphicsView->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}
