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

    void setPlotData(const std::vector<double> &xData, const std::vector<double> &yData);

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

    // Динамические массивы для хранения текущих данных графика
    std::vector<double> m_xData;
    std::vector<double> m_yData;
};
