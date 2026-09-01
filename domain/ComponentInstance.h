#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

#include "ComponentDefinition.h"
#include "PinInstance.h"

#include "ComponentId.h"
#include "PinIndex.h"

// Легковесная структура для замены QPointF в доменной модели
struct Point2D
{
    double x = 0.0;
    double y = 0.0;
};

class ComponentInstance
{
public:
    ComponentInstance(const ComponentDefinition &def, ComponentId id);

    ComponentId getId() const noexcept;

    bool setParameter(const std::string &name, const std::string &value);
    std::string getParameter(const std::string &name) const;

    double getX() const;
    double getY() const;

    const std::string &getType();

    const PinInstance &getPin(PinIndex idx) const;
    PinInstance &getPin(PinIndex idx);
    bool hasPin(PinIndex idx) const;

    PinInstance *findPin(PinIndex idx) { return hasPin(idx) ? &getPin(idx) : nullptr; }

    std::string mType;
    Point2D mPosition;

    ComponentId mId;

    std::vector<std::map<std::string, std::string>> mParameters;
    std::vector<std::unique_ptr<PinInstance>> mPins;
};