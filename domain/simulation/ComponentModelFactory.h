#pragma once
#include <memory>
#include <string>
#include "IComponentModel.h"
#include "LinearModels.h"

class ComponentModelFactory {
public:
    static std::unique_ptr<IComponentModel> create(const std::string& type) {

        if (type == "Laser" || type == "laser") return std::make_unique<LaserModel>();
        if (type == "Fiber" || type == "fiber") return std::make_unique<FiberModel>();

        // По умолчанию для неизвестных компонентов просто пропускаем сигнал (FiberModel)
        return std::make_unique<FiberModel>();
    }
};