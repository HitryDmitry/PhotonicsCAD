#include "Circuit.h"
#include "ComponentFactory.h"
#include "TestHelpers.h"
#include <doctest/doctest.h>

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
        ComponentDefinition fiberDef = TestHelpers::createOpticalFiberDefinition();
        ComponentDefinition eomDef = TestHelpers::createElectroOpticModulatorDefinition();

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
        ComponentDefinition fiberDef = TestHelpers::createOpticalFiberDefinition();
        ComponentDefinition eomDef = TestHelpers::createElectroOpticModulatorDefinition();

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

    TEST_CASE("Connection successful") {}
}

TEST_SUITE("Adding/removing components") {}