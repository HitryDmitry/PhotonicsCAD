#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

#include "ComponentDefinition.h"
#include "PinInstance.h"

// структура для замены QPointF в доменной модели
struct Point2D
{
    double x = 0.0;
    double y = 0.0;
};

class ComponentInstance
{
public:
    ComponentInstance(const ComponentDefinition &def);

    std::string type;
    Point2D position; // Заменили QPointF на Point2D

    // Заменил QVector<QMap<...>> на std::vector<std::map<...>>
    std::vector<std::map<std::string, std::string>> parameters;

    std::vector<std::unique_ptr<PinInstance>> pins;
};