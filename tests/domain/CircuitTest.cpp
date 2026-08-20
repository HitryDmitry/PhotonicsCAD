#include "Circuit.h"
#include "ComponentFactory.h"
#include "TestHelpers.h"
#include <doctest/doctest.h>

using namespace TestHelpers;

TEST_SUITE("Test connection logic")
{
    TEST_CASE("Can't connect pins of the same component")
    {
        Circuit circuit;

        // Формируем два разных идентификатора для входного и выходного пина волокна
        ComponentId fiberId(1);
        PinIndex fiberInputPin(0);
        PinIndex fiberOutputPin(1);
        PinRef fiberInPin{fiberId, fiberInputPin};
        PinRef fiberOutPin{fiberId, fiberOutputPin};

        // Также для EOM
        ComponentId eomId(2);
        PinIndex eomInOpt(0);
        PinIndex eomPinElectr(1);
        PinIndex eomOutOpt(2);
        PinRef eomInPin{eomId, eomInOpt};
        PinRef eomOutPin{eomId, eomOutOpt};

        // Создаем компоненты
        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();

        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 100.0, 200.0);

        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));

        // можно подключиться к другому компоненту
        REQUIRE(circuit.canConnect(fiberOutPin, eomInPin) == true);

        // нельзя подключиться к другим пинам этого же компонента
        REQUIRE(circuit.canConnect(fiberInPin, fiberInPin) == false);
        REQUIRE(circuit.canConnect(eomOutPin, eomOutPin) == false);
        REQUIRE(circuit.canConnect(fiberInPin, fiberOutPin) == false);
        REQUIRE(circuit.canConnect(fiberOutPin, fiberInPin) == false);
        REQUIRE(circuit.canConnect(eomInPin, eomOutPin) == false);
    }

    TEST_CASE("Can't connect pins of different types")
    {
        Circuit circuit;

        // Формируем два разных идентификатора для входного и выходного пина волокна
        ComponentId fiberId(1);
        PinIndex fiberInputPin(0);
        PinIndex fiberOutputPin(1);
        PinRef fiberInPin{fiberId, fiberInputPin};
        PinRef fiberOutPin{fiberId, fiberOutputPin};

        // Также для EOM
        ComponentId eomId(2);
        PinIndex eomInOpt(0);
        PinIndex eomPinElectr(1);
        PinIndex eomOutOpt(2);
        PinRef eomInPin{eomId, eomInOpt};
        PinRef eomControlPin{eomId, eomPinElectr};
        PinRef eomOutPin{eomId, eomOutOpt};

        // Создаем компоненты
        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();

        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 100.0, 200.0);

        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));

        // нельзя подключить оптический выход к электрическому
        REQUIRE(circuit.canConnect(eomControlPin, fiberOutPin) == false);
        REQUIRE(circuit.canConnect(eomInPin, eomControlPin) == false);

        // нельзя поключить вход к входу или выход к выходу
        REQUIRE(circuit.canConnect(fiberInPin, eomInPin) == false);
        REQUIRE(circuit.canConnect(eomOutPin, fiberOutPin) == false);
    }
}

TEST_SUITE("Adding/removing wires")
{
    TEST_CASE("AddWire - should add valid wire")
    {
        Circuit circuit;

        // Создаем id компонентов
        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);

        // Создаем компоненты
        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();
        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 100.0, 200.0);

        // Добавляем в схему
        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));

        PinRef pinA{fiberId, makePinIndex(1)};
        PinRef pinB{eomId, makePinIndex(0)};

        bool result = circuit.addWire(pinA, pinB);

        CHECK(result == true);
        CHECK(circuit.getWireCount() == 1);
    }

    TEST_CASE("AddWire - should handle multiple wires")
    {
        Circuit circuit;

        // Создаем id компонентов
        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);
        auto laserId = makeComponentId(3);

        // Создаем компоненты
        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();
        ComponentDefinition laserDef = createLaserDefinition();
        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 700.0, 900.0);
        auto laserInst = ComponentFactory::createComponent(laserDef, laserId, 300.0, 400.0);

        // Добавляем в схему
        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));
        circuit.addComponent(std::move(laserInst));

        PinRef pinA{laserId, makePinIndex(0)};
        PinRef pinB{fiberId, makePinIndex(0)};
        PinRef pinC{fiberId, makePinIndex(1)};
        PinRef pinD{eomId, makePinIndex(0)};

        CHECK(circuit.addWire(pinA, pinB));
        CHECK(circuit.addWire(pinC, pinD));

        CHECK(circuit.getWireCount() == 2);
    }

    TEST_CASE("AddWire - should handle canonical ordering")
    {
        Circuit circuit;
        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);

        // Создаем компоненты
        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();
        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 100.0, 200.0);

        // Добавляем в схему
        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));

        PinRef pinA{fiberId, makePinIndex(1)};
        PinRef pinB{eomId, makePinIndex(0)};

        // Добавляем в разных порядках
        circuit.addWire(pinA, pinB);
        bool resultReverse = circuit.addWire(pinB, pinA);

        CHECK(resultReverse == false);
        CHECK(circuit.getWireCount() == 1);
    }

    TEST_CASE("AddWire - should not add duplicate wires")
    {
        Circuit circuit;
        // Создаем id компонентов
        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);

        // Создаем компоненты
        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();
        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 100.0, 200.0);

        // Добавляем в схему
        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));

        PinRef pinA{makeComponentId(1), makePinIndex(1)};
        PinRef pinB{makeComponentId(2), makePinIndex(0)};

        circuit.addWire(pinA, pinB);

        bool resultDuplicate = circuit.addWire(pinA, pinB);

        CHECK(resultDuplicate == false);
        CHECK(circuit.getWireCount() == 1);
    }

    TEST_CASE("RemoveWire - should remove existing wire")
    {
        Circuit circuit;
        // Создаем id компонентов
        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);

        // Создаем компоненты
        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();
        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 100.0, 200.0);

        // Добавляем в схему
        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));

        PinRef pinA{fiberId, makePinIndex(1)};
        PinRef pinB{eomId, makePinIndex(0)};

        circuit.addWire(pinA, pinB);
        bool result = circuit.removeWire(pinA, pinB);

        CHECK(result == true);
        CHECK(circuit.getWireCount() == 0);
    }

    TEST_CASE("RemoveWire - should handle canonical ordering")
    {
        Circuit circuit;
        // Создаем id компонентов
        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);

        // Создаем компоненты
        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();
        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 100.0, 200.0);

        // Добавляем в схему
        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));

        PinRef pinA{fiberId, makePinIndex(1)};
        PinRef pinB{eomId, makePinIndex(0)};

        circuit.addWire(pinA, pinB);
        bool result = circuit.removeWire(pinB, pinA);

        CHECK(result == true);
        CHECK(circuit.getWireCount() == 0);
    }

    TEST_CASE("RemoveWire - should return false for non-existing wire")
    {
        Circuit circuit;

        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);

        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();
        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 100.0, 200.0);

        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));

        PinRef pinA{fiberId, makePinIndex(1)};
        PinRef pinB{eomId, makePinIndex(0)};

        bool result = circuit.removeWire(pinA, pinB);

        CHECK(result == false);
        CHECK(circuit.getWireCount() == 0);
    }
}

TEST_SUITE("Adding/removing components")
{
    TEST_CASE("AddComponent - should add single component")
    {
        Circuit circuit;
        auto fiberId = makeComponentId(1);

        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        auto fiber = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);

        bool result = circuit.addComponent(std::move(fiber));

        CHECK(result == true);
        CHECK(circuit.getComponentCount() == 1);

        auto *found = circuit.findComponent(fiberId);
        REQUIRE(found != nullptr);
        CHECK(found->getId() == fiberId);
    }

    TEST_CASE("AddComponent - should add multiple components")
    {
        Circuit circuit;

        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);
        auto laserId = makeComponentId(3);

        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();
        ComponentDefinition laserDef = createLaserDefinition();
        auto fiber = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eom = ComponentFactory::createComponent(eomDef, eomId, 700.0, 900.0);
        auto laser = ComponentFactory::createComponent(laserDef, laserId, 300.0, 400.0);

        CHECK(circuit.addComponent(std::move(fiber)));
        CHECK(circuit.addComponent(std::move(eom)));
        CHECK(circuit.addComponent(std::move(laser)));

        CHECK(circuit.getComponentCount() == 3);
        CHECK(circuit.findComponent(makeComponentId(1)) != nullptr);
        CHECK(circuit.findComponent(makeComponentId(2)) != nullptr);
        CHECK(circuit.findComponent(makeComponentId(3)) != nullptr);
    }

    TEST_CASE("AddComponent - components with same ID should not be added")
    {
        Circuit circuit;

        auto fiberId = makeComponentId(1);

        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        auto fiber = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto fiberDuplicate = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);

        CHECK(circuit.addComponent(std::move(fiber)));
        CHECK(circuit.addComponent(std::move(fiberDuplicate)) == false);

        CHECK(circuit.getComponentCount() == 1);

        auto *found = circuit.findComponent(fiberId);
        CHECK(found != nullptr);
    }

    TEST_CASE("AddComponent - should handle empty component")
    {
        Circuit circuit;

        bool result = circuit.addComponent(nullptr);

        CHECK(result == false);
        CHECK(circuit.getComponentCount() == 0);
    }

    TEST_CASE("RemoveComponent - should remove existing component")
    {
        Circuit circuit;
        auto fiberId = makeComponentId(1);
        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        auto fiber = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);

        circuit.addComponent(std::move(fiber));

        bool result = circuit.removeComponent(fiberId);

        CHECK(result == true);
        CHECK(circuit.getComponentCount() == 0);
        CHECK(circuit.findComponent(makeComponentId(1)) == nullptr);
    }

    TEST_CASE("RemoveComponent - should remove correct component from multiple")
    {
        Circuit circuit;
        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);
        auto laserId = makeComponentId(3);

        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();
        ComponentDefinition laserDef = createLaserDefinition();
        auto fiber = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eom = ComponentFactory::createComponent(eomDef, eomId, 700.0, 900.0);
        auto laser = ComponentFactory::createComponent(laserDef, laserId, 300.0, 400.0);

        circuit.addComponent(std::move(fiber));
        circuit.addComponent(std::move(eom));
        circuit.addComponent(std::move(laser));

        bool result = circuit.removeComponent(makeComponentId(2));

        CHECK(result == true);
        CHECK(circuit.getComponentCount() == 2);
        CHECK(circuit.findComponent(makeComponentId(1)) != nullptr);
        CHECK(circuit.findComponent(makeComponentId(2)) == nullptr);
        CHECK(circuit.findComponent(makeComponentId(3)) != nullptr);
    }

    TEST_CASE("RemoveComponent - should return false for non-existing component")
    {
        Circuit circuit;
        auto fiberId = makeComponentId(1);
        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        auto fiber = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);

        circuit.addComponent(std::move(fiber));

        bool result = circuit.removeComponent(makeComponentId(999));

        CHECK(result == false);
        CHECK(circuit.getComponentCount() == 1);
    }

    TEST_CASE("RemoveComponent - should handle removal from empty circuit")
    {
        Circuit circuit;

        bool result = circuit.removeComponent(makeComponentId(1));

        CHECK(result == false);
        CHECK(circuit.getComponentCount() == 0);
    }

    TEST_CASE("Circuit - should handle component removal with wires")
    {
        Circuit circuit;

        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);
        auto laserId = makeComponentId(3);

        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        ComponentDefinition eomDef = createElectroOpticModulatorDefinition();
        ComponentDefinition laserDef = createLaserDefinition();
        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 700.0, 900.0);
        auto laserInst = ComponentFactory::createComponent(laserDef, laserId, 300.0, 400.0);

        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));
        circuit.addComponent(std::move(laserInst));

        PinRef pinA{laserId, makePinIndex(0)};
        PinRef pinB{fiberId, makePinIndex(0)};
        PinRef pinC{fiberId, makePinIndex(1)};
        PinRef pinD{eomId, makePinIndex(0)};

        CHECK(circuit.addWire(pinA, pinB));
        CHECK(circuit.addWire(pinC, pinD));

        // Удаляем компонент, и привязанные к нему провода должны удалиться
        circuit.removeComponent(fiberId);

        CHECK(circuit.getComponentCount() == 2);
        CHECK(circuit.getWireCount() == 0);
    }

    TEST_CASE("Circuit - should validate pin existence")
    {
        Circuit circuit;
        auto fiberId = makeComponentId(1);

        ComponentDefinition fiberDef = createOpticalFiberDefinition();
        auto fiber = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);

        circuit.addComponent(std::move(fiber));

        PinRef validPin{fiberId, makePinIndex(0)};
        PinRef invalidPin{fiberId, makePinIndex(5)}; // Несуществующий пин

        // В текущей реализации findPin выбросит исключение
        CHECK_THROWS_AS(circuit.canConnect(validPin, invalidPin), std::out_of_range);
    }

    TEST_CASE("Circuit - should handle nullptr from findComponent")
    {
        Circuit circuit;

        PinRef pinA{makeComponentId(999), makePinIndex(0)};
        PinRef pinB{makeComponentId(2), makePinIndex(1)};

        CHECK(circuit.canConnect(pinA, pinB) == false);
    }
}

TEST_SUITE("Circuit - Wire and Pin synchronization")
{
    // Вспомогательная функция для создания компонентов в тестах
    auto createTestComponents = [](Circuit &circuit) {
        auto fiberId = makeComponentId(1);
        auto eomId = makeComponentId(2);
        auto laserId = makeComponentId(3);

        auto fiberDef = createOpticalFiberDefinition();
        auto eomDef = createElectroOpticModulatorDefinition();
        auto laserDef = createLaserDefinition();

        auto fiberInst = ComponentFactory::createComponent(fiberDef, fiberId, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, eomId, 700.0, 900.0);
        auto laserInst = ComponentFactory::createComponent(laserDef, laserId, 300.0, 400.0);

        circuit.addComponent(std::move(fiberInst));
        circuit.addComponent(std::move(eomInst));
        circuit.addComponent(std::move(laserInst));

        return std::tuple{fiberId, eomId, laserId};
    };

    TEST_CASE("AddWire should update pins")
    {
        Circuit circuit;
        auto [fiberId, eomId, laserId] = createTestComponents(circuit);

        // Получаем указатели на пины ДО добавления провода
        auto *laserComp = circuit.findComponent(laserId);
        auto *fiberComp = circuit.findComponent(fiberId);

        REQUIRE(laserComp != nullptr);
        REQUIRE(fiberComp != nullptr);

        auto *pinLaser = laserComp->findPin(makePinIndex(0));
        auto *pinFiber = fiberComp->findPin(makePinIndex(0));

        REQUIRE(pinLaser != nullptr);
        REQUIRE(pinFiber != nullptr);

        // Проверяем, что пины не имеют проводов
        CHECK(pinLaser->getWireCount() == 0);
        CHECK(pinFiber->getWireCount() == 0);

        // Добавляем провод
        PinRef pinA{laserId, makePinIndex(0)};
        PinRef pinB{fiberId, makePinIndex(0)};

        CHECK(circuit.addWire(pinA, pinB));

        // Проверяем, что пины обновились
        CHECK(pinLaser->getWireCount() == 1);
        CHECK(pinFiber->getWireCount() == 1);

        // Проверяем, что провод правильно связан с пинами
        auto *wire = circuit.findWire(pinA, pinB);
        REQUIRE(wire != nullptr);

        // Проверяем, что провод есть в множествах пинов
        CHECK(pinLaser->hasWire(wire) == true);
        CHECK(pinFiber->hasWire(wire) == true);
    }

    TEST_CASE("RemoveWire should update pins")
    {
        Circuit circuit;
        auto [fiberId, eomId, laserId] = createTestComponents(circuit);

        // Получаем указатели на пины
        auto *laserComp = circuit.findComponent(laserId);
        auto *fiberComp = circuit.findComponent(fiberId);

        REQUIRE(laserComp != nullptr);
        REQUIRE(fiberComp != nullptr);

        auto *pinLaser = laserComp->findPin(makePinIndex(0));
        auto *pinFiber = fiberComp->findPin(makePinIndex(0));

        PinRef pinA{laserId, makePinIndex(0)};
        PinRef pinB{fiberId, makePinIndex(0)};

        // Добавляем провод
        circuit.addWire(pinA, pinB);
        CHECK(pinLaser->getWireCount() == 1);
        CHECK(pinFiber->getWireCount() == 1);

        // Удаляем провод
        CHECK(circuit.removeWire(pinA, pinB));

        // Пин должен быть обновлен
        CHECK(pinLaser->getWireCount() == 0);
        CHECK(pinFiber->getWireCount() == 0);

        // Провод больше не должен существовать
        auto *wire = circuit.findWire(pinA, pinB);
        CHECK(wire == nullptr);
    }

    TEST_CASE("RemoveWire should handle reverse order")
    {
        Circuit circuit;
        auto [fiberId, eomId, laserId] = createTestComponents(circuit);

        PinRef pinA{laserId, makePinIndex(0)};
        PinRef pinB{fiberId, makePinIndex(0)};

        // Добавляем провод в прямом порядке
        circuit.addWire(pinA, pinB);
        CHECK(circuit.getWireCount() == 1);

        // Удаляем в обратном порядке - должно работать
        CHECK(circuit.removeWire(pinB, pinA));
        CHECK(circuit.getWireCount() == 0);
    }

    TEST_CASE("RemoveComponent should remove all wires and update pins")
    {
        Circuit circuit;
        auto [fiberId, eomId, laserId] = createTestComponents(circuit);

        // Получаем указатели на пины
        auto *laserComp = circuit.findComponent(laserId);
        auto *fiberComp = circuit.findComponent(fiberId);
        auto *eomComp = circuit.findComponent(eomId);

        REQUIRE(laserComp != nullptr);
        REQUIRE(fiberComp != nullptr);
        REQUIRE(eomComp != nullptr);

        auto *pinLaser0 = laserComp->findPin(makePinIndex(0));
        auto *pinFiber0 = fiberComp->findPin(makePinIndex(0));
        auto *pinFiber1 = fiberComp->findPin(makePinIndex(1));
        auto *pinEom0 = eomComp->findPin(makePinIndex(0));

        // Создаем сложную сеть
        PinRef pinA{laserId, makePinIndex(0)};
        PinRef pinB{fiberId, makePinIndex(0)};
        PinRef pinC{fiberId, makePinIndex(1)};
        PinRef pinD{eomId, makePinIndex(0)};

        circuit.addWire(pinA, pinB);
        circuit.addWire(pinC, pinD);

        // Проверяем состояние пинов
        CHECK(pinLaser0->getWireCount() == 1);
        CHECK(pinFiber0->getWireCount() == 1);
        CHECK(pinFiber1->getWireCount() == 1);
        CHECK(pinEom0->getWireCount() == 1);
        CHECK(circuit.getWireCount() == 2);

        // Удаляем компонент fiber
        bool result = circuit.removeComponent(fiberId);
        CHECK(result == true);

        // Все провода должны быть удалены
        CHECK(circuit.getWireCount() == 0);

        // Пины компонента fiber больше не существуют (компонент удален)
        // Но пины других компонентов должны быть очищены
        CHECK(pinLaser0->getWireCount() == 0);
        CHECK(pinEom0->getWireCount() == 0);

        // Проверяем, что компонент fiber действительно удален
        CHECK(circuit.findComponent(fiberId) == nullptr);

        // Другие компоненты должны существовать
        CHECK(circuit.findComponent(laserId) != nullptr);
        CHECK(circuit.findComponent(eomId) != nullptr);

        // Количество компонентов должно уменьшиться
        CHECK(circuit.getComponentCount() == 2);
    }

    TEST_CASE("RemoveComponent should handle multiple wires on same pin")
    {
        Circuit circuit;
        auto [fiberId, eomId, laserId] = createTestComponents(circuit);

        auto secondFiberId = makeComponentId(fiberId.value() + 1);

        // Создаем дополнительные компоненты
        auto detectorId = makeComponentId(4);
        auto detectorDef = createPhotodetectorDefinition(); // предположим, что такая есть
        auto detectorInst = ComponentFactory::createComponent(detectorDef, detectorId, 500.0, 600.0);
        circuit.addComponent(std::move(detectorInst));

        // Получаем указатели на пины
        auto *laserComp = circuit.findComponent(laserId);
        auto *fiberComp = circuit.findComponent(fiberId);

        REQUIRE(laserComp != nullptr);
        REQUIRE(fiberComp != nullptr);

        auto *pinLaser0 = laserComp->findPin(makePinIndex(0));
        auto *pinFiber0 = fiberComp->findPin(makePinIndex(0));

        // Подключаем несколько проводов к одному пину лазера
        PinRef pinLaser{laserId, makePinIndex(0)};
        PinRef pinFiber1{fiberId, makePinIndex(0)};
        PinRef pinFiber2{secondFiberId, makePinIndex(0)};
        PinRef pinDetector{detectorId, makePinIndex(0)};

        circuit.addWire(pinLaser, pinFiber1);
        circuit.addWire(pinLaser, pinFiber2);
        circuit.addWire(pinLaser, pinDetector);

        // Проверяем, что у пина лазера 3 провода
        CHECK(pinLaser0->getWireCount() == 3);
        CHECK(circuit.getWireCount() == 3);

        // Удаляем один провод
        circuit.removeWire(pinLaser, pinFiber1);
        CHECK(pinLaser0->getWireCount() == 2);
        CHECK(pinFiber0->getWireCount() == 0);
        CHECK(circuit.getWireCount() == 2);

        // Удаляем компонент лазер - все оставшиеся провода должны быть удалены
        circuit.removeComponent(laserId);
        CHECK(circuit.getWireCount() == 0);
        CHECK(circuit.findComponent(laserId) == nullptr);

        // Проверяем, что у других компонентов нет висячих ссылок
        auto *fiberCompAfter = circuit.findComponent(fiberId);
        auto *detectorComp = circuit.findComponent(detectorId);

        if (fiberCompAfter) {
            CHECK(fiberCompAfter->findPin(makePinIndex(0))->getWireCount() == 0);
            CHECK(fiberCompAfter->findPin(makePinIndex(1))->getWireCount() == 0);
        }

        if (detectorComp) {
            CHECK(detectorComp->findPin(makePinIndex(0))->getWireCount() == 0);
        }
    }

    TEST_CASE("RemoveComponent should handle component with no wires")
    {
        Circuit circuit;
        auto [fiberId, eomId, laserId] = createTestComponents(circuit);

        // Не добавляем проводов

        // Удаляем компонент
        CHECK(circuit.removeComponent(fiberId));

        // Проверяем, что компонент удален
        CHECK(circuit.findComponent(fiberId) == nullptr);
        CHECK(circuit.getComponentCount() == 2);
        CHECK(circuit.getWireCount() == 0);

        // Другие компоненты должны существовать
        CHECK(circuit.findComponent(laserId) != nullptr);
        CHECK(circuit.findComponent(eomId) != nullptr);
    }

    TEST_CASE("RemoveComponent should handle non-existent component")
    {
        Circuit circuit;
        auto [fiberId, eomId, laserId] = createTestComponents(circuit);

        // Пытаемся удалить несуществующий компонент
        auto nonExistentId = makeComponentId(999);
        CHECK(circuit.removeComponent(nonExistentId) == false);

        // Количество компонентов не изменилось
        CHECK(circuit.getComponentCount() == 3);
        CHECK(circuit.getWireCount() == 0);
    }

    TEST_CASE("AddWire should not create wire with invalid pins")
    {
        Circuit circuit;
        auto [fiberId, eomId, laserId] = createTestComponents(circuit);

        // Несуществующий индекс пина
        PinRef invalidPin{laserId, makePinIndex(99)};
        PinRef validPin{fiberId, makePinIndex(0)};

        CHECK_THROWS_AS(circuit.addWire(validPin, invalidPin), std::out_of_range);
        CHECK(circuit.getWireCount() == 0);

        // Несуществующий компонент
        PinRef nonExistentComp{makeComponentId(999), makePinIndex(0)};
        CHECK(circuit.addWire(nonExistentComp, validPin) == false);
        CHECK(circuit.getWireCount() == 0);
    }

    TEST_CASE("AddWire should prevent duplicate wires")
    {
        Circuit circuit;
        auto [fiberId, eomId, laserId] = createTestComponents(circuit);

        PinRef pinA{laserId, makePinIndex(0)};
        PinRef pinB{fiberId, makePinIndex(0)};

        // Первое добавление - успешно
        CHECK(circuit.addWire(pinA, pinB));
        CHECK(circuit.getWireCount() == 1);

        // Второе добавление - должно быть отклонено
        CHECK(circuit.addWire(pinA, pinB) == false);
        CHECK(circuit.getWireCount() == 1);

        // Добавление в обратном порядке - тоже должно быть отклонено
        CHECK(circuit.addWire(pinB, pinA) == false);
        CHECK(circuit.getWireCount() == 1);
    }

    TEST_CASE("Circuit should maintain consistency when removing component with wires")
    {
        Circuit circuit;
        auto [fiberId, eomId, laserId] = createTestComponents(circuit);

        // Создаем несколько проводов
        PinRef pinA{laserId, makePinIndex(0)};
        PinRef pinB{fiberId, makePinIndex(0)};
        PinRef pinC{fiberId, makePinIndex(1)};
        PinRef pinD{eomId, makePinIndex(0)};

        circuit.addWire(pinA, pinB);
        circuit.addWire(pinC, pinD);

        // Создаем еще один компонент и провод
        auto secondLaserId = makeComponentId(4);
        auto secondLaserDef = createLaserDefinition();
        auto secondLaserInst = ComponentFactory::createComponent(secondLaserDef,
                                                                 secondLaserId,
                                                                 200.0,
                                                                 300.0);
        circuit.addComponent(std::move(secondLaserInst));

        PinRef pinE{secondLaserId, makePinIndex(0)};
        PinRef pinF{fiberId, makePinIndex(0)}; // Снова к fiber
        circuit.addWire(pinE, pinF);

        CHECK(circuit.getWireCount() == 3);
        CHECK(circuit.getComponentCount() == 4);

        // Получаем пины до удаления
        auto *laserComp = circuit.findComponent(laserId);
        auto *eomComp = circuit.findComponent(eomId);
        auto *secondLaserComp = circuit.findComponent(secondLaserId);

        auto *pinLaser = laserComp->findPin(makePinIndex(0));
        auto *pinEom = eomComp->findPin(makePinIndex(0));
        auto *pinSecondLaser = secondLaserComp->findPin(makePinIndex(0));

        CHECK(pinLaser->getWireCount() == 1);
        CHECK(pinEom->getWireCount() == 1);
        CHECK(pinSecondLaser->getWireCount() == 1);

        // Удаляем fiber компонент (у него 3 провода)
        circuit.removeComponent(fiberId);

        // Все провода должны быть удалены
        CHECK(circuit.getWireCount() == 0);

        // Пины других компонентов должны быть очищены
        CHECK(pinLaser->getWireCount() == 0);
        CHECK(pinEom->getWireCount() == 0);
        CHECK(pinSecondLaser->getWireCount() == 0);

        // Проверяем количество компонентов
        CHECK(circuit.getComponentCount() == 3); // laser, eom, laser2

        // Проверяем, что можно добавить новые провода после удаления
        PinRef newPinA{laserId, makePinIndex(0)};
        PinRef newPinB{eomId, makePinIndex(0)};

        CHECK(circuit.addWire(newPinA, newPinB));
        CHECK(circuit.getWireCount() == 1);

        // И пины обновились корректно
        CHECK(pinLaser->getWireCount() == 1);
        CHECK(pinEom->getWireCount() == 1);
    }
}
