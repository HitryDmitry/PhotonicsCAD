#include "CircuitViewModel.h"
#include "ComponentViewModel.h"
#include "TestHelpers.h"
#include <doctest/doctest.h>

// Мок-слушатель для тестирования уведомлений
class MockCircuitObserver : public ICircuitObserver
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

class MockComponentObserver : public IComponentObserver
{
public:
    explicit MockComponentObserver(ComponentViewModel &subject)
        : mSubject(subject)
    {
        mSubject.addObserver(this);
    }
    void onPropertyModyfied() override {}
    void getDefaultParamsForBuildingUI() {}
    void applyChanges() {}

    ComponentViewModel &mSubject;
};

TEST_SUITE("ViewModel Layer - CircuitViewModel")
{
    TEST_CASE("CircuitViewModel should notify observers on adding a component")
    {
        CircuitViewModel vm;
        MockCircuitObserver observer;

        vm.addObserver(&observer);

        ComponentDefinition def = TestHelpers::createLaserDefinition();

        vm.addComponent(def, 100.0f, 200.0f);

        CHECK(observer.inst->mPosition.x == 100.0f);
        CHECK(observer.inst->mPosition.y == 200.0f);
        REQUIRE(observer.componentTypes.size() == 1);
        CHECK(observer.componentTypes[0] == "laser");
    }
}

TEST_SUITE("ViewModel Layer - ComponentViewModel")
{
    TEST_CASE("Test ComponentViewModel modifies value correctly")
    {
        // вспомогательный код для создания указателя на ComponentInstance
        CircuitViewModel vm;
        MockCircuitObserver circObserver;
        vm.addObserver(&circObserver);
        ComponentDefinition def = TestHelpers::createLaserDefinition();
        vm.addComponent(def, 100.0f, 200.0f);

        // Инициализация ComponentViewModel и наблюдателя (PropertyEditorDialog)
        ComponentViewModel cvm(circObserver.inst);

        std::string keyStd("key");
        std::string propertyToChange("power");
        std::string newValue("5.0");

        cvm.modifyProperty(propertyToChange, newValue);

        for (auto &paramInst : cvm.getInstanceParamsVector()) {
            if (paramInst.at(keyStd) == propertyToChange) {
                auto it = paramInst.find("default");
                if (it != paramInst.end()) {
                    REQUIRE(it->second == newValue);
                }
                break;
            }
        }
    }
}