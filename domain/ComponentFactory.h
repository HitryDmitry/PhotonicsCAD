//Он отвечает за инкапсуляцию логики создания компонентов
#pragma once
#include <memory>
#include "ComponentInstance.h"
#include "ComponentDefinition.h"

class ComponentFactory
{
public:
    static std::unique_ptr<ComponentInstance> createComponent(const ComponentDefinition &def, double x, double y)
    {
        auto instance = std::make_unique<ComponentInstance>(def);
        instance->mPosition = Point2D{x, y};
        instance->mType = def.type.toStdString();
        return instance;
    }
};