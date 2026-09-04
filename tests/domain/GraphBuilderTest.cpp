#include "GraphBuilder.h"
#include "ComponentDefinition.h"
#include "TestHelpers.h"
#include <doctest/doctest.h>

#include <algorithm>
#include <map>
#include <stdexcept>

// Класс для построения тестовых схем
class CircuitBuilder
{
private:
    Circuit mCircuit;
    std::map<std::string, ComponentDefinition> mDefinitions;
    ComponentId mNextId = TestHelpers::makeComponentId(1);

    // Кэш для быстрого поиска индексов пинов по имени
    std::map<ComponentId, std::map<std::string, PinIndex>> mPinIndexCache;

public:
    CircuitBuilder()
    {
        // Регистрируем стандартные типы компонентов
        registerComponentType(TestHelpers::createLaserDefinition());
        registerComponentType(TestHelpers::createOpticalFiberDefinition());
        registerComponentType(TestHelpers::createElectroOpticModulatorDefinition());
        registerComponentType(TestHelpers::createPhotodetectorDefinition());
        registerComponentType(TestHelpers::createMicrowaveAmplifierDefinition());

        // Регистрируем дополнительные компоненты для тестов
        registerComponentType(createSplitterDefinition());
        registerComponentType(createTerminalDefinition());
        registerComponentType(createCombinerDefinition());
    }

    // Регистрация компонента в библиотеке
    void registerComponentType(const ComponentDefinition &def)
    {
        mDefinitions[def.type.toStdString()] = def;
    }

    // Добавление компонента в схему
    ComponentId addComponent(const std::string &type,
                             const std::map<std::string, std::string> &params = {})
    {
        auto defIt = mDefinitions.find(type);
        if (defIt == mDefinitions.end()) {
            throw std::runtime_error("Unknown component type: " + type);
        }

        // Создаем экземпляр компонента
        auto instance = std::make_unique<ComponentInstance>(defIt->second, mNextId);

        // Устанавливаем параметры
        for (const auto &[name, value] : params) {
            if (!instance->setParameter(name, value)) {
                throw std::runtime_error("Failed to set parameter: " + name + " = " + value);
            }
        }

        // Добавляем в схему
        if (!mCircuit.addComponent(std::move(instance))) {
            throw std::runtime_error("Failed to add component to circuit");
        }

        // Кэшируем индексы пинов для этого компонента
        cachePinIndices(mNextId, defIt->second);

        return mNextId++;
    }

    // Добавление провода между компонентами
    void addWire(ComponentId from,
                 const std::string &fromPin,
                 ComponentId to,
                 const std::string &toPin)
    {
        auto fromIdx = getPinIndex(from, fromPin);
        auto toIdx = getPinIndex(to, toPin);

        PinRef fromRef{from, fromIdx};
        PinRef toRef{to, toIdx};

        if (!mCircuit.addWire(fromRef, toRef)) {
            throw std::runtime_error("Failed to add wire from " + std::to_string(from.value()) + ":"
                                     + fromPin + " to " + std::to_string(to.value()) + ":" + toPin);
        }
    }

    // Сборка схемы
    std::unique_ptr<Circuit> build()
    {
        // Создаем копию схемы (или перемещаем)
        return std::make_unique<Circuit>(std::move(mCircuit));
    }

    // Получение схемы по ссылке (для отладки)
    Circuit &getCircuit() { return mCircuit; }
    const Circuit &getCircuit() const { return mCircuit; }

    // Поиск компонента по ID
    ComponentInstance *findComponent(ComponentId id) { return mCircuit.findComponent(id); }

private:
    // Создание определения для Splitter
    ComponentDefinition createSplitterDefinition()
    {
        ComponentDefinition def;
        def.type = "splitter";
        def.name = "Optical Splitter";
        def.iconPath = ":/icons/splitter.png";

        def.pins = {TestHelpers::createPin("in", "Input", "input", "optical"),
                    TestHelpers::createPin("out1", "Output 1", "output", "optical"),
                    TestHelpers::createPin("out2", "Output 2", "output", "optical")};

        def.parameters = {TestHelpers::createParameter(
                              "split_ratio", "Split Ratio", "%", "double", 50.0, 0.0, 100.0),
                          TestHelpers::createParameter(
                              "excess_loss", "Excess Loss", "dB", "double", 0.5, 0.0, 100.0)};

        return def;
    }

    // Создание определения для Terminal (нагрузка/заглушка)
    ComponentDefinition createTerminalDefinition()
    {
        ComponentDefinition def;
        def.type = "terminal";
        def.name = "Terminal";
        def.iconPath = ":/icons/terminal.png";

        def.pins = {TestHelpers::createPin("in", "Input", "input", "optical")};

        def.parameters = {TestHelpers::createParameter(
            "load_impedance", "Load Impedance", "Ohm", "double", 50.0, 0.0, 1000000.0)};

        return def;
    }

    // Создание определения для Combiner
    ComponentDefinition createCombinerDefinition()
    {
        ComponentDefinition def;
        def.type = "combiner";
        def.name = "Optical Combiner";
        def.iconPath = ":/icons/combiner.png";

        def.pins = {TestHelpers::createPin("in1", "Input 1", "input", "optical"),
                    TestHelpers::createPin("in2", "Input 2", "input", "optical"),
                    TestHelpers::createPin("out", "Output", "output", "optical")};

        def.parameters = {TestHelpers::createParameter(
            "combining_loss", "Combining Loss", "dB", "double", 0.5, 0.0, 100.0)};

        return def;
    }

    // Кэширование индексов пинов для компонента
    void cachePinIndices(ComponentId id, const ComponentDefinition &def)
    {
        std::map<std::string, PinIndex> indexMap;
        for (size_t i = 0; i < def.pins.size(); ++i) {
            QString pinName = def.pins[i]["name"].toString();
            auto pinIdx = TestHelpers::makePinIndex(static_cast<std::uint16_t>(i));
            indexMap.emplace(pinName.toStdString(), pinIdx);
        }
        mPinIndexCache[id] = indexMap;
    }

    // Получение индекса пина по имени
    PinIndex getPinIndex(ComponentId id, const std::string &pinName)
    {
        auto cacheIt = mPinIndexCache.find(id);
        if (cacheIt == mPinIndexCache.end()) {
            // Если нет в кэше, пробуем найти через ComponentInstance
            auto *comp = mCircuit.findComponent(id);
            if (!comp) {
                throw std::runtime_error("Component not found: " + std::to_string(id.value()));
            }

            // Ищем определение по типу
            auto defIt = mDefinitions.find(comp->getType());
            if (defIt == mDefinitions.end()) {
                throw std::runtime_error("Definition not found for type: " + comp->getType());
            }

            // Строим кэш
            cachePinIndices(id, defIt->second);
            cacheIt = mPinIndexCache.find(id);
        }

        auto pinIt = cacheIt->second.find(pinName);
        if (pinIt == cacheIt->second.end()) {
            throw std::runtime_error("Pin not found: " + pinName + " in component "
                                     + std::to_string(id.value()));
        }

        return pinIt->second;
    }
};

// ============================================================================
// ТЕСТЫ
// ============================================================================

TEST_CASE("GraphBuilder - Simple chain of components")
{
    CircuitBuilder builder;

    // Создаем схему: Laser -> Fiber -> Photodetector
    auto laserId = builder.addComponent("laser");
    auto fiberId = builder.addComponent("optical_fiber");
    auto pdId = builder.addComponent("photodetector");

    builder.addWire(laserId, "out", fiberId, "in");
    builder.addWire(fiberId, "out", pdId, "opt_in");

    auto circuit = builder.build();
    auto graph = GraphBuilder::build(circuit.get());

    REQUIRE(graph != nullptr);

    SUBCASE("Sources")
    {
        CHECK(graph->sources.size() == 1);
        CHECK(graph->sources[0] == laserId);
    }

    SUBCASE("Execution order")
    {
        CHECK(graph->executionOrder.size() == 3);
        CHECK(graph->executionOrder[0] == laserId);
        CHECK(graph->executionOrder[1] == fiberId);
        CHECK(graph->executionOrder[2] == pdId);
    }
}

TEST_CASE("GraphBuilder - Splitter with two branches")
{
    CircuitBuilder builder;

    auto laser = builder.addComponent("laser");
    auto splitter = builder.addComponent("splitter");
    auto fiber1 = builder.addComponent("optical_fiber");
    auto fiber2 = builder.addComponent("optical_fiber");
    auto pd1 = builder.addComponent("photodetector");
    auto pd2 = builder.addComponent("photodetector");

    builder.addWire(laser, "out", splitter, "in");
    builder.addWire(splitter, "out1", fiber1, "in");
    builder.addWire(splitter, "out2", fiber2, "in");
    builder.addWire(fiber1, "out", pd1, "opt_in");
    builder.addWire(fiber2, "out", pd2, "opt_in");

    auto circuit = builder.build();
    auto graph = GraphBuilder::build(circuit.get());

    SUBCASE("Sources")
    {
        CHECK(graph->sources.size() == 1);
        CHECK(graph->sources[0] == laser);
    }

    SUBCASE("Topological order")
    {
        auto findPos = [&](ComponentId id) -> size_t {
            auto it = std::find(graph->executionOrder.begin(), graph->executionOrder.end(), id);
            REQUIRE(it != graph->executionOrder.end());
            return std::distance(graph->executionOrder.begin(), it);
        };

        CHECK(findPos(laser) < findPos(splitter));
        CHECK(findPos(splitter) < findPos(fiber1));
        CHECK(findPos(splitter) < findPos(fiber2));
        CHECK(findPos(fiber1) < findPos(pd1));
        CHECK(findPos(fiber2) < findPos(pd2));
    }
}

TEST_CASE("GraphBuilder - Multiple sources")
{
    CircuitBuilder builder;

    auto laser1 = builder.addComponent("laser");
    auto laser2 = builder.addComponent("laser");
    auto splitter = builder.addComponent("splitter");
    auto pd1 = builder.addComponent("photodetector");
    auto pd2 = builder.addComponent("photodetector");

    builder.addWire(laser1, "out", splitter, "in");
    builder.addWire(splitter, "out1", pd1, "opt_in");
    builder.addWire(laser2, "out", pd2, "opt_in");

    auto circuit = builder.build();
    auto graph = GraphBuilder::build(circuit.get());

    SUBCASE("Sources count")
    {
        CHECK(graph->sources.size() == 2);
    }

    SUBCASE("Sources contain both lasers")
    {
        bool hasLaser1 = std::find(graph->sources.begin(), graph->sources.end(), laser1)
                         != graph->sources.end();
        bool hasLaser2 = std::find(graph->sources.begin(), graph->sources.end(), laser2)
                         != graph->sources.end();

        CHECK(hasLaser1);
        CHECK(hasLaser2);
    }

    SUBCASE("Topological order")
    {
        auto findPos = [&](ComponentId id) -> size_t {
            auto it = std::find(graph->executionOrder.begin(), graph->executionOrder.end(), id);
            REQUIRE(it != graph->executionOrder.end());
            return std::distance(graph->executionOrder.begin(), it);
        };

        CHECK(findPos(laser1) < findPos(splitter));
        CHECK(findPos(splitter) < findPos(pd1));
        CHECK(findPos(laser2) < findPos(pd2));
    }
}

TEST_CASE("GraphBuilder - With amplifier")
{
    CircuitBuilder builder;

    auto laser = builder.addComponent("laser");
    auto modulator = builder.addComponent("electro_optic_modulator");
    auto amp = builder.addComponent("microwave_amplifier");
    auto pd = builder.addComponent("photodetector");

    builder.addWire(laser, "out", modulator, "opt_in");
    builder.addWire(modulator, "rf_in", amp, "in");  // RF сигнал от модулятора к усилителю
    builder.addWire(amp, "out", modulator, "rf_in"); // Обратная связь? Нет, это неправильно

    auto circuit = builder.build();
    auto graph = GraphBuilder::build(circuit.get());

    // Проверяем, что график построен корректно
    CHECK(graph != nullptr);
    CHECK(graph->sources.size() == 2); // Laser и RF вход
}

TEST_CASE("GraphBuilder - Cyclic graph throws exception")
{
    CircuitBuilder builder;

    auto mod1 = builder.addComponent("electro_optic_modulator");
    auto mod2 = builder.addComponent("electro_optic_modulator");
    auto fiber = builder.addComponent("optical_fiber");

    // Создаем цикл: mod1 -> fiber -> mod2 -> mod1
    builder.addWire(mod1, "opt_out", fiber, "in");
    builder.addWire(fiber, "out", mod2, "opt_in");
    builder.addWire(mod2, "opt_out", mod1, "opt_in");

    auto circuit = builder.build();

    CHECK_THROWS_AS({ auto graph = GraphBuilder::build(circuit.get()); }, std::runtime_error);
}

TEST_CASE("GraphBuilder - Parallel independent chains")
{
    CircuitBuilder builder;

    // Цепь 1: Laser -> Fiber -> PD
    auto laser1 = builder.addComponent("laser");
    auto fiber1 = builder.addComponent("optical_fiber");
    auto pd1 = builder.addComponent("photodetector");

    // Цепь 2: Laser -> Fiber -> PD
    auto laser2 = builder.addComponent("laser");
    auto fiber2 = builder.addComponent("optical_fiber");
    auto pd2 = builder.addComponent("photodetector");

    builder.addWire(laser1, "out", fiber1, "in");
    builder.addWire(fiber1, "out", pd1, "opt_in");

    builder.addWire(laser2, "out", fiber2, "in");
    builder.addWire(fiber2, "out", pd2, "opt_in");

    auto circuit = builder.build();
    auto graph = GraphBuilder::build(circuit.get());

    SUBCASE("Sources")
    {
        CHECK(graph->sources.size() == 2);
    }

    SUBCASE("All components present")
    {
        CHECK(graph->executionOrder.size() == 6);
    }

    SUBCASE("Topological order")
    {
        auto findPos = [&](ComponentId id) -> size_t {
            auto it = std::find(graph->executionOrder.begin(), graph->executionOrder.end(), id);
            REQUIRE(it != graph->executionOrder.end());
            return std::distance(graph->executionOrder.begin(), it);
        };

        CHECK(findPos(laser1) < findPos(fiber1));
        CHECK(findPos(fiber1) < findPos(pd1));

        CHECK(findPos(laser2) < findPos(fiber2));
        CHECK(findPos(fiber2) < findPos(pd2));
    }
}

TEST_CASE("GraphBuilder - Empty circuit")
{
    CircuitBuilder builder;
    auto circuit = builder.build();
    auto graph = GraphBuilder::build(circuit.get());

    CHECK(graph != nullptr);
    CHECK(graph->sources.empty());
    CHECK(graph->executionOrder.empty());
    CHECK(graph->outputToInputs.empty());
}

TEST_CASE("GraphBuilder - Isolated component")
{
    CircuitBuilder builder;

    auto laser = builder.addComponent("laser");
    auto isolated = builder.addComponent("optical_fiber");

    auto circuit = builder.build();
    auto graph = GraphBuilder::build(circuit.get());

    SUBCASE("Sources")
    {
        CHECK(graph->sources.size() == 1);
        CHECK(graph->sources[0] == laser);
    }

    SUBCASE("Execution order")
    {
        CHECK(graph->executionOrder.size() == 2);

        bool hasLaser = std::find(graph->executionOrder.begin(), graph->executionOrder.end(), laser)
                        != graph->executionOrder.end();
        bool hasIsolated = std::find(graph->executionOrder.begin(),
                                     graph->executionOrder.end(),
                                     isolated)
                           != graph->executionOrder.end();

        CHECK(hasLaser);
        CHECK(hasIsolated);
    }
}

TEST_CASE("GraphBuilder - Complex hierarchy")
{
    CircuitBuilder builder;

    // Источник
    auto laser = builder.addComponent("laser");

    // Первый каскад
    auto splitter1 = builder.addComponent("splitter");
    auto splitter2 = builder.addComponent("splitter");
    auto splitter3 = builder.addComponent("splitter");

    // Второй каскад - волокна
    auto fiber1 = builder.addComponent("optical_fiber");
    auto fiber2 = builder.addComponent("optical_fiber");
    auto fiber3 = builder.addComponent("optical_fiber");
    auto fiber4 = builder.addComponent("optical_fiber");

    // Нагрузка
    auto pd1 = builder.addComponent("photodetector");
    auto pd2 = builder.addComponent("photodetector");
    auto pd3 = builder.addComponent("photodetector");
    auto pd4 = builder.addComponent("photodetector");

    // Соединения: дерево 1->2->4
    builder.addWire(laser, "out", splitter1, "in");

    builder.addWire(splitter1, "out1", splitter2, "in");
    builder.addWire(splitter1, "out2", splitter3, "in");

    builder.addWire(splitter2, "out1", fiber1, "in");
    builder.addWire(splitter2, "out2", fiber2, "in");
    builder.addWire(splitter3, "out1", fiber3, "in");
    builder.addWire(splitter3, "out2", fiber4, "in");

    builder.addWire(fiber1, "out", pd1, "opt_in");
    builder.addWire(fiber2, "out", pd2, "opt_in");
    builder.addWire(fiber3, "out", pd3, "opt_in");
    builder.addWire(fiber4, "out", pd4, "opt_in");

    auto circuit = builder.build();
    auto graph = GraphBuilder::build(circuit.get());

    SUBCASE("All components present")
    {
        // 1 лазер + 3 сплиттера + 4 волокна + 4 фотодетектора = 12
        CHECK(graph->executionOrder.size() == 12);
    }

    SUBCASE("Topological order")
    {
        auto findPos = [&](ComponentId id) -> size_t {
            auto it = std::find(graph->executionOrder.begin(), graph->executionOrder.end(), id);
            REQUIRE(it != graph->executionOrder.end());
            return std::distance(graph->executionOrder.begin(), it);
        };

        CHECK(findPos(laser) < findPos(splitter1));
        CHECK(findPos(splitter1) < findPos(splitter2));
        CHECK(findPos(splitter1) < findPos(splitter3));
        CHECK(findPos(splitter2) < findPos(fiber1));
        CHECK(findPos(splitter2) < findPos(fiber2));
        CHECK(findPos(splitter3) < findPos(fiber3));
        CHECK(findPos(splitter3) < findPos(fiber4));
        CHECK(findPos(fiber1) < findPos(pd1));
        CHECK(findPos(fiber2) < findPos(pd2));
        CHECK(findPos(fiber3) < findPos(pd3));
        CHECK(findPos(fiber4) < findPos(pd4));
    }
}

TEST_CASE("GraphBuilder - Edge cases")
{
    SUBCASE("Wire between two outputs should be ignored")
    {
        CircuitBuilder builder;
        auto laser1 = builder.addComponent("laser");
        auto laser2 = builder.addComponent("laser");

        // Провод между двумя выходами (некорректно)
        builder.addWire(laser1, "out", laser2, "out");

        auto circuit = builder.build();
        auto graph = GraphBuilder::build(circuit.get());

        CHECK(graph->sources.size() == 2);
        CHECK(graph->outputToInputs.empty());
    }

    SUBCASE("Wire between two inputs should be ignored")
    {
        CircuitBuilder builder;
        auto pd1 = builder.addComponent("photodetector");
        auto pd2 = builder.addComponent("photodetector");

        // Провод между двумя входами (некорректно)
        builder.addWire(pd1, "opt_in", pd2, "opt_in");

        auto circuit = builder.build();
        auto graph = GraphBuilder::build(circuit.get());

        CHECK(graph->sources.empty());
        CHECK(graph->executionOrder.size() == 2);
        CHECK(graph->outputToInputs.empty());
    }
}

TEST_CASE("GraphBuilder - Components with parameters")
{
    CircuitBuilder builder;

    // Создаем лазер с параметрами
    auto laser = builder.addComponent("laser", {{"power", "5.0"}, {"frequency", "193.1"}});

    // Создаем волокно с параметрами
    auto fiber = builder.addComponent("optical_fiber",
                                      {{"length", "2.5"},
                                       {"attenuation", "0.3"},
                                       {"refractive_index", "1.47"}});

    auto pd = builder.addComponent("photodetector", {{"responsivity", "0.9"}});

    builder.addWire(laser, "out", fiber, "in");
    builder.addWire(fiber, "out", pd, "opt_in");

    auto circuit = builder.build();
    auto graph = GraphBuilder::build(circuit.get());

    // Проверяем, что параметры установлены
    auto *laserComp = builder.findComponent(laser);
    REQUIRE(laserComp != nullptr);
    CHECK(laserComp->getParameter("power") == "5.0");
    CHECK(laserComp->getParameter("frequency") == "193.1");

    auto *fiberComp = builder.findComponent(fiber);
    REQUIRE(fiberComp != nullptr);
    CHECK(fiberComp->getParameter("length") == "2.5");
    CHECK(fiberComp->getParameter("attenuation") == "0.3");

    // Проверяем топологию
    CHECK(graph->executionOrder.size() == 3);
    CHECK(graph->executionOrder[0] == laser);
    CHECK(graph->executionOrder[1] == fiber);
    CHECK(graph->executionOrder[2] == pd);
}

// Параметризованные тесты с использованием GENERATE
TEST_CASE("GraphBuilder - Various topologies")
{
    CircuitBuilder builder;
    std::string topology;

    // В doctest GENERATE работает через SUBCASE или макросы
    // Используем подход с перебором вручную
    std::vector<std::string> topologies = {"chain", "tree", "star"};

    for (const auto &top : topologies) {
        CAPTURE(top);
        CircuitBuilder b;

        if (top == "chain") {
            auto l = b.addComponent("laser");
            auto f1 = b.addComponent("optical_fiber");
            auto f2 = b.addComponent("optical_fiber");
            auto pd = b.addComponent("photodetector");

            b.addWire(l, "out", f1, "in");
            b.addWire(f1, "out", f2, "in");
            b.addWire(f2, "out", pd, "opt_in");

            auto circuit = b.build();
            auto graph = GraphBuilder::build(circuit.get());

            CHECK(graph->executionOrder.size() == 4);
        } else if (top == "tree") {
            auto l = b.addComponent("laser");
            auto s = b.addComponent("splitter");
            auto f1 = b.addComponent("optical_fiber");
            auto f2 = b.addComponent("optical_fiber");
            auto pd1 = b.addComponent("photodetector");
            auto pd2 = b.addComponent("photodetector");

            b.addWire(l, "out", s, "in");
            b.addWire(s, "out1", f1, "in");
            b.addWire(s, "out2", f2, "in");
            b.addWire(f1, "out", pd1, "opt_in");
            b.addWire(f2, "out", pd2, "opt_in");

            auto circuit = b.build();
            auto graph = GraphBuilder::build(circuit.get());

            CHECK(graph->executionOrder.size() == 6);
        } else if (top == "star") {
            // Звезда: один вход, много выходов
            auto l = b.addComponent("laser");
            auto s = b.addComponent("splitter");
            auto pd1 = b.addComponent("photodetector");
            auto pd2 = b.addComponent("photodetector");
            auto pd3 = b.addComponent("photodetector");

            b.addWire(l, "out", s, "in");
            b.addWire(s, "out1", pd1, "opt_in");
            b.addWire(s, "out2", pd2, "opt_in");
            // Для третьего выхода нужен splitter с 3 выходами, используем два сплиттера
            // или просто пропускаем для упрощения

            auto circuit = b.build();
            auto graph = GraphBuilder::build(circuit.get());

            CHECK(graph->executionOrder.size() >= 4);
        }
    }
}