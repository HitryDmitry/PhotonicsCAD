#include "CircuitViewModel.h"
#include "ComponentLibraryManager.h"
#include <doctest/doctest.h>

// Мок-слушатель для тестирования уведомлений
class MockObserver : public ICircuitObserver
{
public:
    void onComponentAdded(ComponentInstance *instance, const ComponentDefinition *def) override
    {
        componentTypes.push_back(instance->mType);
        inst = instance;
    }
    ComponentInstance *inst;
    std::vector<std::string> componentTypes;
};

TEST_SUITE("ViewModel Layer - ComponentViewModel")
{
    TEST_CASE("ComponentViewModel should notify observers on adding a component")
    {
        CircuitViewModel vm;
        MockObserver observer;
        ComponentLibraryManager compLibManager;

        compLibManager.loadFromJson(":/data/components.json");
        vm.addObserver(&observer);

        const ComponentDefinition *def = compLibManager.getByType("laser");

        vm.addComponent(*def, 100.0f, 200.0f);

        CHECK(observer.inst->mPosition.x == 100.0f);
        CHECK(observer.inst->mPosition.y == 200.0f);
        REQUIRE(observer.componentTypes.size() == 1);
        CHECK(observer.componentTypes[0] == "laser");
    }
}