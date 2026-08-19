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
