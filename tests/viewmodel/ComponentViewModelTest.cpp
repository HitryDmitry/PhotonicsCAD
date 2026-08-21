#include "ComponentViewModel.h"
#include "CircuitViewModel.h"
#include "TestHelpers.h"
#include <doctest/doctest.h>

using namespace TestHelpers;

class MockComponentObserver : public IComponentObserver
{
public:
    explicit MockComponentObserver(ComponentViewModel &subject)
        : mSubject(subject)
    {
        mSubject.addObserver(this);
    }
    void onPropertyModyfied() override { propertyModified = true; }
    ~MockComponentObserver() override { mSubject.removeObserver(this); }

    // void getDefaultParamsForBuildingUI() {}
    // void applyChanges() {}

    ComponentViewModel &mSubject;
    bool propertyModified = false;
};

TEST_SUITE("ComponentViewModel basic checks")
{
    TEST_CASE("ComponentViewModel creation")
    {
        auto compId = makeComponentId(1);
        CircuitViewModel cvm;

        ComponentViewModel componentVM(compId, &cvm);
    }
}
