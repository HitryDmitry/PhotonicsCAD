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
        PinRef eomControlPin{eomId, eomPinElectr};
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
        CHECK(circuit.getWireKeyCount() == 1);
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
        CHECK(circuit.getWireKeyCount() == 2);
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
        CHECK(circuit.getWireKeyCount() == 1);
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
        CHECK(circuit.getWireKeyCount() == 1);
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
        CHECK(circuit.getWireKeyCount() == 0);
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
        CHECK(circuit.getWireKeyCount() == 0);
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
// TEST_SUITE("Adding/removing components")
// {
//     TEST_CASE("AddComponent - should add single component")
//     {
//         Circuit circuit;
//         auto component = createTestComponent(1);

//         bool result = circuit.addComponent(std::move(component));

//         CHECK(result == true);
//         CHECK(circuit.getComponentCount() == 1);

//         auto *found = circuit.findComponent(makeComponentId(1));
//         REQUIRE(found != nullptr);
//         CHECK(found->getId() == makeComponentId(1));
//     }

//     TEST_CASE("AddComponent - should add multiple components")
//     {
//         Circuit circuit;

//         auto comp1 = createTestComponent(1);
//         auto comp2 = createTestComponent(2);
//         auto comp3 = createTestComponent(3);

//         CHECK(circuit.addComponent(std::move(comp1)));
//         CHECK(circuit.addComponent(std::move(comp2)));
//         CHECK(circuit.addComponent(std::move(comp3)));

//         CHECK(circuit.getComponentCount() == 3);
//         CHECK(circuit.findComponent(makeComponentId(1)) != nullptr);
//         CHECK(circuit.findComponent(makeComponentId(2)) != nullptr);
//         CHECK(circuit.findComponent(makeComponentId(3)) != nullptr);
//     }

//     TEST_CASE("AddComponent - should handle components with same ID")
//     {
//         Circuit circuit;

//         auto comp1 = createTestComponent(1);
//         auto comp2 = createTestComponent(1); // Тот же ID

//         CHECK(circuit.addComponent(std::move(comp1)));
//         CHECK(circuit.addComponent(
//             std::move(comp2))); // Должен добавить (если нет проверки на дубликаты)

//         // Примечание: текущая реализация позволяет дубликаты
//         CHECK(circuit.getComponentCount() == 2);

//         // findComponent вернет первый найденный
//         auto *found = circuit.findComponent(makeComponentId(1));
//         CHECK(found != nullptr);
//     }

//     TEST_CASE("AddComponent - should handle empty component")
//     {
//         Circuit circuit;

//         bool result = circuit.addComponent(nullptr);

//         // В текущей реализации push_back(nullptr) вызовет ошибку компиляции
//         // или неопределенное поведение. Это показывает необходимость проверки.
//         CHECK(result == true); // Текущая реализация не проверяет nullptr
//         CHECK(circuit.getComponentCount() == 1);
//     }

//     TEST_CASE("RemoveComponent - should remove existing component")
//     {
//         Circuit circuit;
//         auto component = createTestComponent(1);
//         circuit.addComponent(std::move(component));

//         bool result = circuit.removeComponent(makeComponentId(1));

//         CHECK(result == true);
//         CHECK(circuit.getComponentCount() == 0);
//         CHECK(circuit.findComponent(makeComponentId(1)) == nullptr);
//     }

//     TEST_CASE("RemoveComponent - should remove correct component from multiple")
//     {
//         Circuit circuit;
//         circuit.addComponent(createTestComponent(1));
//         circuit.addComponent(createTestComponent(2));
//         circuit.addComponent(createTestComponent(3));

//         bool result = circuit.removeComponent(makeComponentId(2));

//         CHECK(result == true);
//         CHECK(circuit.getComponentCount() == 2);
//         CHECK(circuit.findComponent(makeComponentId(1)) != nullptr);
//         CHECK(circuit.findComponent(makeComponentId(2)) == nullptr);
//         CHECK(circuit.findComponent(makeComponentId(3)) != nullptr);
//     }

//     TEST_CASE("RemoveComponent - should return false for non-existing component")
//     {
//         Circuit circuit;
//         circuit.addComponent(createTestComponent(1));

//         bool result = circuit.removeComponent(makeComponentId(999));

//         CHECK(result == false);
//         CHECK(circuit.getComponentCount() == 1);
//     }

//     TEST_CASE("RemoveComponent - should handle removal from empty circuit")
//     {
//         Circuit circuit;

//         bool result = circuit.removeComponent(makeComponentId(1));

//         CHECK(result == false);
//         CHECK(circuit.getComponentCount() == 0);
//     }
//     TEST_CASE("Circuit - should handle component removal with wires")
//     {
//         Circuit circuit;
//         circuit.addComponent(createTestComponent(1, 2));
//         circuit.addComponent(createTestComponent(2, 2));

//         PinRef pinA{makeComponentId(1), makePinIndex(0)};
//         PinRef pinB{makeComponentId(2), makePinIndex(1)};

//         circuit.addWire(pinA, pinB);

//         // Удаляем компонент, но провода остаются (это баг в текущей реализации)
//         circuit.removeComponent(makeComponentId(1));

//         // В идеале провода должны быть удалены, но текущая реализация этого не делает
//         CHECK(circuit.getComponentCount() == 1);
//         CHECK(circuit.getWireCount() == 1); // Провод все еще существует
//     }

//     TEST_CASE("Circuit - should validate pin existence")
//     {
//         Circuit circuit;
//         circuit.addComponent(createTestComponent(1, 2));

//         PinRef validPin{makeComponentId(1), makePinIndex(0)};
//         PinRef invalidPin{makeComponentId(1), makePinIndex(5)}; // Несуществующий пин

//         // В текущей реализации findPin может выбросить исключение
//         CHECK_THROWS_AS(circuit.canConnect(validPin, invalidPin), std::out_of_range);
//     }

//     TEST_CASE("Circuit - should handle nullptr from findComponent")
//     {
//         Circuit circuit;

//         PinRef pinA{makeComponentId(999), makePinIndex(0)};
//         PinRef pinB{makeComponentId(2), makePinIndex(1)};

//         // В текущей реализации будет nullptr dereference
//         // CHECK_THROWS_AS(circuit.canConnect(pinA, pinB), std::runtime_error);
//         // Это показывает необходимость проверки в коде
//     }
// }
