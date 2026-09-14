#include "ImPlotWidget.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include <QTimer>

ImPlotWidget::ImPlotWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    // 1. Задаем жесткую политику размеров, чтобы Layout не сжимал виджет
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(450, 400);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // 2. Вместо update() в paintGL, используем безопасный таймер на 60 FPS
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    timer->start(16); // ~60 кадров в секунду
}

ImPlotWidget::~ImPlotWidget()
{
    // Проверяем, существует ли еще контекст окна Qt
    if (isValid()) {
        makeCurrent();

        // Безопасный вызов Shutdown: проверяем, инициализирован ли бэкенд
        if (ImGui::GetCurrentContext() != nullptr) {
            // Проверяем внутренний флаг ImGui, чтобы не вызвать Shutdown дважды
            if (ImGui::GetIO().BackendRendererName != nullptr) {
                ImGui_ImplOpenGL3_Shutdown();
            }

            // Уничтожаем контексты ImPlot и ImGui
            if (ImPlot::GetCurrentContext() != nullptr) {
                ImPlot::DestroyContext();
            }
            ImGui::DestroyContext();
        }

        doneCurrent();
    }
}

void ImPlotWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Создаем контексты, только если они еще не созданы (актуально для множества окон)
    if (ImGui::GetCurrentContext() == nullptr) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();
    }

    // Инициализируем бэкенд для текущего контекста OpenGL
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

    // 1. Убираем внутренние отступы (padding) окна ImGui,
    // чтобы график прижимался ровно к краям Qt-виджета
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    // Позиционируем окно ImGui ровно под размеры QOpenGLWidget
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width(), height()), ImGuiCond_Always);

    // Обязательно убираем все рамки и фоны самого окна ImGui,
    // чтобы график занимал 100% пространства виджета Qt
    ImGui::Begin("ImPlot Window",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                     | ImGuiWindowFlags_NoCollapse
                     | ImGuiWindowFlags_NoBackground); // Добавлен флаг NoBackground

    // 2. Исправляем выделение памяти: объявляем полноценные массивы на 101 элемент
    static float x_data[101];
    static float y_data[101];
    static bool data_initialized = false;

    if (!data_initialized) {
        for (int i = 0; i <= 100; ++i) {
            x_data[i] = i * 0.1f;
            y_data[i] = sinf(x_data[i]);
        }
        data_initialized = true;
    }

    // 3. Передаем ImVec2(-1, -1) — это заставит ImPlot автоматически
    // растянуться на 100% доступной ширины и высоты окна ImGui
    if (ImPlot::BeginPlot("My First ImPlot Widget", ImVec2(-1, -1))) {
        ImPlot::PlotLine("Sin(x)", x_data, y_data, 101);
        ImPlot::EndPlot();
    }

    ImGui::End();

    // Возвращаем настройки стилей в исходное состояние
    ImGui::PopStyleVar();

    // Рендеринг ImGui сцены
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
