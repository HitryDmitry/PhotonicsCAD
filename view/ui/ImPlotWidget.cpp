#include "ImPlotWidget.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

ImPlotWidget::ImPlotWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    // Разрешаем виджету принимать фокус для обработки клавиатуры
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

ImPlotWidget::~ImPlotWidget()
{
    makeCurrent();
    ImGui_ImplOpenGL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    doneCurrent();
}

void ImPlotWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // 1. Создаем контексты ImGui и ImPlot
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Настройка стиля (опционально)
    ImGui::StyleColorsDark();

    // 2. Инициализируем бэкенд рендеринга (OpenGL 3)
    // В Qt 6 под Windows/Linux/macOS используется профиль не ниже #version 150 или 330 core
    ImGui_ImplOpenGL3_Init("#version 150");

    m_timer.start();
}

void ImPlotWidget::paintGL()
{
    // Очистка экрана силами OpenGL
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Обновляем размеры окна и дельту времени
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(width(), height());
    io.DeltaTime = m_timer.restart() / 1000.0f;

    // Начало нового кадра
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // Позиционируем окно ImGui ровно под размеры QOpenGLWidget
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(width(), height()));

    ImGui::Begin("ImPlot Window",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                     | ImGuiWindowFlags_NoCollapse);

    // --- ОТРИСОВКА ГРАФИКА IMPLOT ---
    if (ImPlot::BeginPlot("My First ImPlot Widget", ImVec2(-1, -1))) {
        static float x_data[101];
        static float y_data[101];
        for (int i = 0; i <= 100; ++i) {
            x_data[i] = i * 0.1f;
            y_data[i] = sinf(x_data[i]);
        }

        ImPlot::PlotLine("Sin(x)", x_data, y_data, 101);
        ImPlot::EndPlot();
    }

    ImGui::End();

    // Рендеринг ImGui сцены
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Постоянно запрашиваем обновление (для интерактивности и плавного зума/перетаскивания)
    update();
}

void ImPlotWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

// --- Передача событий мыши в ImGui ---

void ImPlotWidget::mousePressEvent(QMouseEvent *event)
{
    ImGuiIO &io = ImGui::GetIO();
    if (event->button() == Qt::LeftButton)
        io.MouseDown[0] = true;
    if (event->button() == Qt::RightButton)
        io.MouseDown[1] = true;
    if (event->button() == Qt::MiddleButton)
        io.MouseDown[2] = true;
}

void ImPlotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    ImGuiIO &io = ImGui::GetIO();
    if (event->button() == Qt::LeftButton)
        io.MouseDown[0] = false;
    if (event->button() == Qt::RightButton)
        io.MouseDown[1] = false;
    if (event->button() == Qt::MiddleButton)
        io.MouseDown[2] = false;
}

void ImPlotWidget::mouseMoveEvent(QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPointF pos = event->position();
#else
    QPointF pos = event->localPos();
#endif
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(pos.x(), pos.y());
}

void ImPlotWidget::wheelEvent(QWheelEvent *event)
{
    ImGuiIO &io = ImGui::GetIO();
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta();

    if (!numPixels.isNull()) {
        io.MouseWheel += numPixels.y() / 15.0f;
        io.MouseWheelH += numPixels.x() / 15.0f;
    } else if (!numDegrees.isNull()) {
        io.MouseWheel += numDegrees.y() / 120.0f;
        io.MouseWheelH += numDegrees.x() / 120.0f;
    }
}
