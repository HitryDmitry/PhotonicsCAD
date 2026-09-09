#include "GraphBuilder.h"
#include "CircuitBuilder.h"

#include <doctest/doctest.h>
#include <algorithm>
#include <stdexcept>

TEST_SUITE("Testing correctness of graphs")
{
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
        auto splitter = builder.addComponent("optical_splitter");
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

            // Проверяем порядок вычисления компонентов
            CHECK(graph->executionOrder.at(0).value() == 1);
            CHECK(graph->executionOrder.at(1).value() == 2);
            CHECK(graph->executionOrder.at(2).value() == 4);
            CHECK(graph->executionOrder.at(3).value() == 3);
            CHECK(graph->executionOrder.at(4).value() == 6);
            CHECK(graph->executionOrder.at(5).value() == 5);
        }
    }

    TEST_CASE("GraphBuilder - Multiple sources")
    {
        CircuitBuilder builder;

        auto laser1 = builder.addComponent("laser");
        auto laser2 = builder.addComponent("laser");
        auto splitter = builder.addComponent("optical_splitter");
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

    TEST_CASE("GraphBuilder - Dangling input throws error")
    {
        CircuitBuilder builder;

        auto laser = builder.addComponent("laser");
        auto modulator = builder.addComponent("electro_optic_modulator");
        auto amp = builder.addComponent("microwave_amplifier");
        auto pd = builder.addComponent("photodetector");

        builder.addWire(laser, "out", modulator, "opt_in");
        builder.addWire(modulator, "opt_out", pd, "opt_in");
        builder.addWire(amp, "out", modulator, "rf_in"); // вход усилителя ни к чему не подключен

        auto circuit = builder.build();

        // Проверяем, что выбрасывается исключение
        CHECK_THROWS_AS({ auto graph = GraphBuilder::build(circuit.get()); }, std::runtime_error);
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

    TEST_CASE("GraphBuilder - Complex hierarchy")
    {
        CircuitBuilder builder;

        // Источник
        auto laser = builder.addComponent("laser");

        // Первый каскад
        auto splitter1 = builder.addComponent("optical_splitter");
        auto splitter2 = builder.addComponent("optical_splitter");
        auto splitter3 = builder.addComponent("optical_splitter");

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
        auto circuitPtr = circuit.get();
        auto graph = GraphBuilder::build(circuitPtr);

        // Проверяем, что параметры установлены
        auto *laserComp = circuitPtr->findComponent(laser);
        REQUIRE(laserComp != nullptr);
        CHECK(laserComp->getParameter("power") == "5.0");
        CHECK(laserComp->getParameter("frequency") == "193.1");

        auto *fiberComp = circuitPtr->findComponent(fiber);
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
                auto s = b.addComponent("optical_splitter");
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
                auto s1 = b.addComponent("optical_splitter");
                auto s2 = b.addComponent("optical_splitter");
                auto pd1 = b.addComponent("photodetector");
                auto pd2 = b.addComponent("photodetector");
                auto pd3 = b.addComponent("photodetector");

                b.addWire(l, "out", s1, "in");
                b.addWire(s1, "out1", pd1, "opt_in");
                b.addWire(s1, "out2", s2, "in");
                b.addWire(s2, "out1", pd2, "opt_in");
                b.addWire(s2, "out1", pd3, "opt_in");

                auto circuit = b.build();
                auto graph = GraphBuilder::build(circuit.get());

                CHECK(graph->executionOrder.size() >= 4);
            }
        }
    }

    TEST_CASE("GraphBuilder - Observation points with spectrum analyzer")
    {
        CircuitBuilder builder;

        // Создаем схему: Laser -> Fiber -> Spectrum Analyzer
        auto laser = builder.addComponent("laser");
        auto fiber = builder.addComponent("optical_fiber");
        auto pd = builder.addComponent("photodetector");
        auto esa = builder.addComponent("electrical_spectrum_analyzer");

        builder.addWire(laser, "out", fiber, "in");
        builder.addWire(fiber, "out", pd, "opt_in");
        builder.addWire(pd, "elec_out", esa, "in");

        auto circuit = builder.build();
        auto graph = GraphBuilder::build(circuit.get());

        REQUIRE(graph != nullptr);

        SUBCASE("Observation points count")
        {
            // Анализатор спектра имеет один входной пин, поэтому должен быть добавлен
            CHECK(graph->observationPoints.size() == 1);
        }

        SUBCASE("Observation point is correct")
        {
            // Проверяем, что observationPoint указывает на вход анализатора
            auto &obsPoint = graph->observationPoints[0];

            // Находим компонент с типом electrical_spectrum_analyzer
            bool found = false;
            auto &circuitRef = const_cast<Circuit &>(*circuit);
            for (const auto &compPtr : circuitRef.mComponents) {
                if (compPtr->getType() == "electrical_spectrum_analyzer") {
                    CHECK(obsPoint.componentId == compPtr->getId());
                    found = true;
                    break;
                }
            }
            CHECK(found);
        }
    }

    TEST_CASE("GraphBuilder - Multiple observation points")
    {
        CircuitBuilder builder;

        // Создаем схему с несколькими точками наблюдения
        auto laser = builder.addComponent("laser");
        auto splitter = builder.addComponent("optical_splitter");
        auto esa1 = builder.addComponent("electrical_spectrum_analyzer");
        auto esa2 = builder.addComponent("electrical_spectrum_analyzer");
        auto pd1 = builder.addComponent("photodetector");
        auto pd2 = builder.addComponent("photodetector");

        // Laser -> Splitter -> PD1(2) -> ESA1(2)
        builder.addWire(laser, "out", splitter, "in");
        builder.addWire(splitter, "out1", pd1, "opt_in");
        builder.addWire(splitter, "out2", pd2, "opt_in");
        builder.addWire(pd1, "elec_out", esa1, "in");
        builder.addWire(pd2, "elec_out", esa2, "in");

        auto circuit = builder.build();
        auto graph = GraphBuilder::build(circuit.get());

        REQUIRE(graph != nullptr);

        SUBCASE("Observation points count")
        {
            // Должно быть два анализатора спектра
            CHECK(graph->observationPoints.size() == 2);
        }

        SUBCASE("Different signal types")
        {
            // Проверяем, что оба анализатора присутствуют
            int esaCount = 0;
            auto &circuitRef = const_cast<Circuit &>(*circuit);
            for (const auto &compPtr : circuitRef.mComponents) {
                if (compPtr->getType() == "electrical_spectrum_analyzer") {
                    esaCount++;

                    // Проверяем, что компонент есть в observationPoints
                    bool found = false;
                    for (const auto &obsPoint : graph->observationPoints) {
                        if (obsPoint.componentId == compPtr->getId()) {
                            found = true;
                            break;
                        }
                    }
                    CHECK(found);
                }
            }
            CHECK(esaCount == 2);
        }
    }

    TEST_CASE("GraphBuilder - Components with multiple pins not in observation points")
    {
        CircuitBuilder builder;

        // Компонент с несколькими пинами не должен автоматически добавляться
        // в observationPoints (только если явно указан)
        auto laser = builder.addComponent("laser");
        auto splitter = builder.addComponent("optical_splitter"); // 3 пина (in, out1, out2)
        auto fiber = builder.addComponent("optical_fiber");
        auto pd = builder.addComponent("photodetector");

        builder.addWire(laser, "out", splitter, "in");
        builder.addWire(splitter, "out1", fiber, "in");
        builder.addWire(fiber, "out", pd, "opt_in");

        auto circuit = builder.build();
        auto graph = GraphBuilder::build(circuit.get());

        REQUIRE(graph != nullptr);

        SUBCASE("Splitter not in observation points")
        {
            // Сплиттер имеет 3 пина, поэтому не должен быть в observationPoints
            bool splitterFound = false;
            for (const auto &obsPoint : graph->observationPoints) {
                auto *comp = const_cast<Circuit *>(circuit.get())
                                 ->findComponent(obsPoint.componentId);
                if (comp && comp->getType() == "optical_splitter") {
                    splitterFound = true;
                    break;
                }
            }
            CHECK(!splitterFound);
        }

        SUBCASE("Fiber not in observation points")
        {
            // Волокно имеет 2 пина, поэтому не должно быть в observationPoints
            bool fiberFound = false;
            for (const auto &obsPoint : graph->observationPoints) {
                auto *comp = const_cast<Circuit *>(circuit.get())
                                 ->findComponent(obsPoint.componentId);
                if (comp && comp->getType() == "optical_fiber") {
                    fiberFound = true;
                    break;
                }
            }
            CHECK(!fiberFound);
        }

        SUBCASE("Photodetector not in observation points")
        {
            // Фотодетектор имеет 2 пина, поэтому не должен быть в observationPoints
            bool pdFound = false;
            for (const auto &obsPoint : graph->observationPoints) {
                auto *comp = const_cast<Circuit *>(circuit.get())
                                 ->findComponent(obsPoint.componentId);
                if (comp && comp->getType() == "photodetector") {
                    pdFound = true;
                    break;
                }
            }
            CHECK(!pdFound);
        }
    }
}

TEST_SUITE("Count input/output components")
{
    TEST_CASE("GraphBuilder - Pin count for single-pin components")
    {
        CircuitBuilder builder;

        // Создаем простую цепочку: Laser (1 пин) -> Fiber (2 пина) -> PD (2 пина)
        auto laser = builder.addComponent("laser");         // 1 пин (output)
        auto fiber = builder.addComponent("optical_fiber"); // 2 пина (input, output)
        auto pd = builder.addComponent("photodetector");    // 2 пина (opt_in, elec_out)

        builder.addWire(laser, "out", fiber, "in");
        builder.addWire(fiber, "out", pd, "opt_in");

        auto circuit = builder.build();
        auto graph = GraphBuilder::build(circuit.get());

        REQUIRE(graph != nullptr);

        SUBCASE("Laser pin counts")
        {
            // Лазер имеет 1 выходной пин и 0 входных
            auto itIn = graph->numInPinsPerComp.find(laser);
            auto itOut = graph->numOutPinsPerComp.find(laser);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            CHECK(itIn->second == 0);  // нет входных пинов
            CHECK(itOut->second == 1); // один выходной пин
        }

        SUBCASE("Fiber pin counts")
        {
            // Волокно имеет 1 входной и 1 выходной пин
            auto itIn = graph->numInPinsPerComp.find(fiber);
            auto itOut = graph->numOutPinsPerComp.find(fiber);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            CHECK(itIn->second == 1);  // один входной пин
            CHECK(itOut->second == 1); // один выходной пин
        }

        SUBCASE("Photodetector pin counts")
        {
            // Фотодетектор имеет 1 входной и 1 выходной пин
            auto itIn = graph->numInPinsPerComp.find(pd);
            auto itOut = graph->numOutPinsPerComp.find(pd);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            CHECK(itIn->second == 1);  // один входной пин (opt_in)
            CHECK(itOut->second == 1); // один выходной пин (elec_out)
        }
    }

    TEST_CASE("GraphBuilder - Pin count for splitter with branches")
    {
        CircuitBuilder builder;

        // Создаем схему с разветвителем: Laser -> Splitter -> (Fiber1 -> PD1, Fiber2 -> PD2)
        auto laser = builder.addComponent("laser");               // 1 пин (output)
        auto splitter = builder.addComponent("optical_splitter"); // 3 пина (in, out1, out2)
        auto fiber1 = builder.addComponent("optical_fiber");      // 2 пина (input, output)
        auto fiber2 = builder.addComponent("optical_fiber");      // 2 пина (input, output)
        auto pd1 = builder.addComponent("photodetector");         // 2 пина (opt_in, elec_out)
        auto pd2 = builder.addComponent("photodetector");         // 2 пина (opt_in, elec_out)

        builder.addWire(laser, "out", splitter, "in");
        builder.addWire(splitter, "out1", fiber1, "in");
        builder.addWire(splitter, "out2", fiber2, "in");
        builder.addWire(fiber1, "out", pd1, "opt_in");
        builder.addWire(fiber2, "out", pd2, "opt_in");

        auto circuit = builder.build();
        auto graph = GraphBuilder::build(circuit.get());

        REQUIRE(graph != nullptr);

        SUBCASE("Laser pin counts")
        {
            auto itIn = graph->numInPinsPerComp.find(laser);
            auto itOut = graph->numOutPinsPerComp.find(laser);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            CHECK(itIn->second == 0);
            CHECK(itOut->second == 1);
        }

        SUBCASE("Splitter pin counts")
        {
            // Сплиттер имеет 1 вход и 2 выхода
            auto itIn = graph->numInPinsPerComp.find(splitter);
            auto itOut = graph->numOutPinsPerComp.find(splitter);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            CHECK(itIn->second == 1);  // один входной пин (in)
            CHECK(itOut->second == 2); // два выходных пина (out1, out2)
        }

        SUBCASE("Fiber1 pin counts")
        {
            auto itIn = graph->numInPinsPerComp.find(fiber1);
            auto itOut = graph->numOutPinsPerComp.find(fiber1);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            CHECK(itIn->second == 1);
            CHECK(itOut->second == 1);
        }

        SUBCASE("Fiber2 pin counts")
        {
            auto itIn = graph->numInPinsPerComp.find(fiber2);
            auto itOut = graph->numOutPinsPerComp.find(fiber2);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            CHECK(itIn->second == 1);
            CHECK(itOut->second == 1);
        }

        SUBCASE("Photodetector 1 pin counts")
        {
            auto itIn = graph->numInPinsPerComp.find(pd1);
            auto itOut = graph->numOutPinsPerComp.find(pd1);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            CHECK(itIn->second == 1);
            CHECK(itOut->second == 1);
        }
    }

    TEST_CASE("GraphBuilder - Pin count for components with multiple inputs")
    {
        CircuitBuilder builder;

        // Создаем схему с комбайнером (2 входа, 1 выход)
        auto laser1 = builder.addComponent("laser");              // 1 пин
        auto laser2 = builder.addComponent("laser");              // 1 пин
        auto combiner = builder.addComponent("optical_combiner"); // 3 пина (in1, in2, out)
        auto pd = builder.addComponent("photodetector");          // 2 пина

        builder.addWire(laser1, "out", combiner, "in1");
        builder.addWire(laser2, "out", combiner, "in2");
        builder.addWire(combiner, "out", pd, "opt_in");

        auto circuit = builder.build();
        auto graph = GraphBuilder::build(circuit.get());

        REQUIRE(graph != nullptr);

        SUBCASE("Laser1 pin counts")
        {
            auto itIn = graph->numInPinsPerComp.find(laser1);
            auto itOut = graph->numOutPinsPerComp.find(laser1);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            CHECK(itIn->second == 0);
            CHECK(itOut->second == 1);
        }

        SUBCASE("Combiner pin counts")
        {
            // Комбайнер имеет 2 входа и 1 выход
            auto itIn = graph->numInPinsPerComp.find(combiner);
            auto itOut = graph->numOutPinsPerComp.find(combiner);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            CHECK(itIn->second == 2);  // два входных пина (in1, in2)
            CHECK(itOut->second == 1); // один выходной пин (out)
        }

        SUBCASE("Total pins calculation")
        {
            // Проверяем, что сумма входных и выходных пинов равна общему количеству пинов
            for (const auto &[compId, inCount] : graph->numInPinsPerComp) {
                auto outIt = graph->numOutPinsPerComp.find(compId);
                REQUIRE(outIt != graph->numOutPinsPerComp.end());

                auto *comp = const_cast<Circuit *>(circuit.get())->findComponent(compId);
                REQUIRE(comp != nullptr);

                size_t totalPins = comp->mPins.size();
                CHECK((inCount + outIt->second) == totalPins);
            }
        }
    }

    TEST_CASE("GraphBuilder - Pin count for components with unconnected pins")
    {
        CircuitBuilder builder;

        // Создаем схему, где некоторые пины не подключены
        auto laser = builder.addComponent("laser");               // 1 пин
        auto splitter = builder.addComponent("optical_splitter"); // 3 пина
        auto fiber = builder.addComponent("optical_fiber");       // 2 пина
        auto pd = builder.addComponent("photodetector");          // 2 пина

        // Подключаем только один выход сплиттера
        builder.addWire(laser, "out", splitter, "in");
        builder.addWire(splitter, "out1", fiber, "in");
        builder.addWire(fiber, "out", pd, "opt_in");
        // out2 сплиттера остается неподключенным

        auto circuit = builder.build();
        auto graph = GraphBuilder::build(circuit.get());

        REQUIRE(graph != nullptr);

        SUBCASE("Splitter with unconnected output")
        {
            // Сплиттер имеет 1 вход и 1 подключенный выход (out2 не подключен)
            // В текущей реализации GraphBuilder считает только подключенные входные пины
            // и любые (подключенные/неподключенные) пины
            auto itIn = graph->numInPinsPerComp.find(splitter);
            auto itOut = graph->numOutPinsPerComp.find(splitter);

            REQUIRE(itIn != graph->numInPinsPerComp.end());
            REQUIRE(itOut != graph->numOutPinsPerComp.end());

            // Входной пин подключен
            CHECK(itIn->second == 1);

            // Только один выходной пин подключен (out1)
            CHECK(itOut->second == 2);

            // Общее количество пинов компонента - 3
            auto *comp = const_cast<Circuit *>(circuit.get())->findComponent(splitter);
            REQUIRE(comp != nullptr);
            CHECK((itIn->second + itOut->second) == 3);
        }

        SUBCASE("All components in execution order")
        {
            // Все компоненты должны быть в графе
            CHECK(graph->executionOrder.size() == 4);
        }
    }

    TEST_CASE("GraphBuilder - Pin count validation throws on dangling input")
    {
        CircuitBuilder builder;

        // Создаем схему с висячим входом (компонент с входным пином, не подключенным к источнику)
        auto laser = builder.addComponent("laser");
        auto fiber = builder.addComponent("optical_fiber"); // у fiber есть входной пин in
        auto pd = builder.addComponent("photodetector");

        // Подключаем только выход fiber -> pd, но не подключаем laser -> fiber
        // builder.addWire(laser, "out", fiber, "in"); // Пропускаем это соединение
        builder.addWire(fiber, "out", pd, "opt_in");

        auto circuit = builder.build();

        // Должно быть выброшено исключение из-за висячего входа
        CHECK_THROWS_AS({ auto graph = GraphBuilder::build(circuit.get()); }, std::runtime_error);

        // Проверяем сообщение об ошибке
        try {
            auto graph = GraphBuilder::build(circuit.get());
        } catch (const std::runtime_error &e) {
            std::string errorMsg = e.what();
            CHECK(errorMsg.find("Dangling input") != std::string::npos);
        }
    }
}