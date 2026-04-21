#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMetaMethod>
#include "ComponentDefinition.h"
#include "ComponentInstance.h"
#include "GraphicsView.h"
#include "PropertyEditorDialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->listWidget->setDragEnabled(true);
    ui->listWidget->setDragDropMode(ComponentListWidget::DragOnly);

    ui->graphicsView->setAcceptDrops(true);
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);

    // Создаем сцену
    m_scene = new QGraphicsScene(this);
    // Устанавливаем сцену в GraphicsView из ui
    ui->graphicsView->setScene(m_scene);

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

    // Не понятно, почему не работает обновленный синтаксис связи сигналов-слотов
    // qDebug() << ui->graphicsView->metaObject()->className();
    // qDebug() << ui->listWidget->metaObject()->className();
    // for (int i = 0; i < ui->graphicsView->metaObject()->methodCount(); ++i) {
    //     qDebug() << ui->graphicsView->metaObject()->method(i).methodSignature();
    // }
    // connect(ui->graphicsView,
    //         &GraphicsView::componentDropped,
    //         this,
    //         &MainWindow::onComponentDropped);

    // устаревший синтаксис работает
    connect(ui->graphicsView,
            SIGNAL(componentDropped(QString, QPointF)),
            this,
            SLOT(onComponentDropped(QString, QPointF)));
}

void MainWindow::onComponentDropped(const QString &type, const QPointF &pos)
{
    const ComponentDefinition *def = componentLibrary.getByType(type);

    if (!def) {
        qDebug() << "Can't get ComponentDefinition from componentLibrary by type.";
    }

    ComponentInstance *compInstance = new ComponentInstance(*def);

    if (compInstance == nullptr) {
        qDebug() << "ComponentInstance is nullptr!";
    }

    auto item = new GraphicsComponentItem(compInstance, def);
    // connect(item,
    //         &GraphicsComponentItem::doubleClicked,
    //         this,
    //         &MainWindow::onComponentDoubleClicked);
    connect(item,
            SIGNAL(doubleClicked(ComponentInstance *)),
            this,
            SLOT(onComponentDoubleClicked(ComponentInstance *)));

    item->setPos(pos);

    m_scene->addItem(item);
    ui->graphicsView->viewport()->update();
    ui->graphicsView->update();
    ui->graphicsView->show();
}

// void MainWindow::onComponentMoved(const QString &type, const QPointF &newPos)
// {
//     ui->graphicsView->viewport()->update();
//     ui->graphicsView->update();
//     ui->graphicsView->show();
// }

void MainWindow::onItemSelected(GraphicsComponentItem *item)
{
    ComponentInstance *instance = item->getInstance();

    if (!instance) {
        qDebug() << "Can't retrieve ComponentInstance from selected GraphicsComponentItem.";
    }

    qDebug() << "Position: " << instance->position;
}

void MainWindow::onComponentDoubleClicked(ComponentInstance *instance)
{
    const ComponentDefinition *def = componentLibrary.getByType(instance->type);

    if (!def)
        return;

    PropertyEditorDialog dialog(instance, def, this);
    dialog.exec();
    // item->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}