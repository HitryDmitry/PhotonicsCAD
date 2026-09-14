#pragma once

#include <QElapsedTimer>
#include <QMouseEvent>
#include <QOpenGLExtraFunctions>
#include <QOpenGLWidget>
#include <QWheelEvent>

class ImPlotWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit ImPlotWidget(QWidget *parent = nullptr);
    ~ImPlotWidget();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    // Обработка ввода для ImGui/ImPlot
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void updateImGuiInput();
    QElapsedTimer m_timer;
};
