#include "ComponentFactory.h"
#include "ComponentModelFactory.h"
#include "TestHelpers.h"
#include <doctest/doctest.h>

using namespace TestHelpers;

TEST_SUITE("Testing components creation")
{
    TEST_CASE("Create laser")
    {
        auto laserId = makeComponentId(3);
        ComponentDefinition laserDef = createLaserDefinition();
        auto laserData = ComponentFactory::createComponent(laserDef, laserId, 300.0, 400.0);

        auto laserModel = ComponentModelFactory::create(laserData.get());
    }
}

TEST_SUITE("Testing transfer functions calculation")
{
    TEST_CASE("Test laser transfer function") {}
}
