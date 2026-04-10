#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "ComponentLibraryManager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ComponentLibraryManager library;
    library.loadFromJson(":/data/components.json");

    for (const auto &comp : library.getComponents()) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(comp.iconPath), comp.name);

        item->setData(Qt::UserRole, comp.type);
        ui->listWidget->addItem(item);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
