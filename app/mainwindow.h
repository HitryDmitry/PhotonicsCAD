#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ComponentLibraryManager.h"
#include "GraphicsComponentItem.h"

class CircuitScene;
class Circuit;

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

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void escButtonPressed();

public slots:
    void onComponentDropped(const QString &type, const QPointF &pos);
    void onComponentDoubleClicked(ComponentInstance *instance);
    // void onComponentMoved(const QString &type, const QPointF &newPos);

private:
    Ui::MainWindow *ui;
    CircuitScene *m_scene;
    ComponentLibraryManager componentLibrary;
    std::unique_ptr<Circuit> currentCircuit;
};
#endif // MAINWINDOW_H
