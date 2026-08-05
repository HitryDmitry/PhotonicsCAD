#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

#include "ComponentDefinition.h"
#include "PinInstance.h"

// Легковесная структура для замены QPointF в доменной модели
struct Point2D
{
    double x = 0.0;
    double y = 0.0;
};

class ComponentInstance
{
public:
    ComponentInstance(const ComponentDefinition &def);

    std::string mType;
    Point2D mPosition;

    std::vector<std::map<std::string, std::string>> mParameters;
    std::vector<std::unique_ptr<PinInstance>> mPins;
};