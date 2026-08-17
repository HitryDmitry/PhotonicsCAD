#include "Circuit.h"
#include "ComponentFactory.h"
#include "TestHelpers.h"
#include <doctest/doctest.h>

TEST_SUITE("Test connection logic")
{
    TEST_CASE("Can't connect pins of the same component")
    {
        Circuit circuit;

        ComponentDefinition fiberDef = TestHelpers::createOpticalFiberDefinition();
        ComponentDefinition eomDef = TestHelpers::createElectroOpticModulatorDefinition();

        auto fiberInst = ComponentFactory::createComponent(fiberDef, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, 100.0, 200.0);

        ComponentInstance *fiberRawPtr = fiberInst.get();
        ComponentInstance *eomRawPtr = eomInst.get();

        circuit.addComponent();
        circuit.addComponent();

        // Формируем два разных идентификатора для входного и выходного пина волокна
        ComponentId fiberId(1);
        PinIndex fiberInputPin(1);
        PinIndex fiberOutputPin(2);
        PinRef fiberInPin{fiberId, fiberInputPin};
        PinRef fiberOutPin{fiberId, fiberOutputPin};

        // Также для EOM
        ComponentId eomId(2);
        PinIndex eomInOpt(1);
        PinIndex eomPinElectr(2);
        PinIndex eomOutOpt(3);
        PinRef eomInPin{eomId, eomInOpt};
        PinRef eomControlPin{eomId, eomPinElectr};
        PinRef eomOutPin{eomId, eomOutOpt};

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

        ComponentDefinition fiberDef = TestHelpers::createOpticalFiberDefinition();
        ComponentDefinition eomDef = TestHelpers::createElectroOpticModulatorDefinition();

        auto fiberInst = ComponentFactory::createComponent(fiberDef, 100.0, 200.0);
        auto eomInst = ComponentFactory::createComponent(eomDef, 100.0, 200.0);

        ComponentInstance *fiberRawPtr = fiberInst.get();
        ComponentInstance *eomRawPtr = eomInst.get();

        circuit.addComponent();
        circuit.addComponent();

        // Формируем два разных идентификатора для входного и выходного пина волокна
        ComponentId fiberId(1);
        PinIndex fiberInputPin(1);
        PinIndex fiberOutputPin(2);
        PinRef fiberInPin{fiberId, fiberInputPin};
        PinRef fiberOutPin{fiberId, fiberOutputPin};

        // Также для EOM
        ComponentId eomId(2);
        PinIndex eomInOpt(1);
        PinIndex eomPinElectr(2);
        PinIndex eomOutOpt(3);
        PinRef eomInPin{eomId, eomInOpt};
        PinRef eomControlPin{eomId, eomPinElectr};
        PinRef eomOutPin{eomId, eomOutOpt};

        // нельзя подключить оптический выход к электрическому
        REQUIRE(circuit.canConnect(eomControlPin, fiberOutPin) == false);
        REQUIRE(circuit.canConnect(eomInPin, eomControlPin) == false);

        // нельзя поключить вход к входу или выход к выходу
        REQUIRE(circuit.canConnect(fiberInPin, eomInPin) == false);
        REQUIRE(circuit.canConnect(eomOutPin, fiberOutPin) == false);
    }

    TEST_CASE("Connection successful") {}
}