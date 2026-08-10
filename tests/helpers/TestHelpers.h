#pragma once
#include "ComponentDefinition.h"

namespace TestHelpers {

// Структура для параметра
inline QMap<QString, QVariant> createParameter(const QString &key,
                                               const QString &name,
                                               const QString &unit,
                                               const QString &datatype,
                                               double defaultValue,
                                               double minValue,
                                               double maxValue)
{
    QMap<QString, QVariant> param;
    param["key"] = key;
    param["name"] = name;
    param["unit"] = unit;
    param["datatype"] = datatype;
    param["default"] = defaultValue;
    param["min"] = minValue;
    param["max"] = maxValue;
    return param;
}

// Структура для пина
inline QMap<QString, QVariant> createPin(const QString &id,
                                         const QString &name,
                                         const QString &direction,
                                         const QString &signalType)
{
    QMap<QString, QVariant> pin;
    pin["id"] = id;
    pin["name"] = name;
    pin["direction"] = direction;
    pin["signal_type"] = signalType;
    return pin;
}

// Создание Laser компонента
inline ComponentDefinition createLaserDefinition()
{
    ComponentDefinition def;
    def.type = "laser";
    def.name = "Laser";
    def.iconPath = ":/icons/laser.png";

    // Пины
    def.pins = {createPin("out", "Optical Output", "output", "optical")};

    // Параметры
    def.parameters
        = {createParameter("power", "Laser Power", "mW", "double", 1.0, 0.0, 1000.0),
           createParameter("frequency", "Frequency", "GHz", "double", 193.5, 0.0, 10000.0),
           createParameter("rin_noise", "RIN Noise", "dB/Hz", "double", -150.0, -300.0, 0.0)};

    return def;
}

// Создание Optical Fiber компонента
inline ComponentDefinition createOpticalFiberDefinition()
{
    ComponentDefinition def;
    def.type = "optical_fiber";
    def.name = "Optical Fiber";
    def.iconPath = ":/icons/optical_fiber.png";

    // Пины
    def.pins = {createPin("in", "Input", "input", "optical"),
                createPin("out", "Output", "output", "optical")};

    // Параметры
    def.parameters
        = {createParameter("length", "Length", "km", "double", 1.0, 0.0, 10000.0),
           createParameter("attenuation", "Attenuation", "dB/km", "double", 0.2, 0.0, 100.0),
           createParameter("refractive_index", "Refractive Index", "", "double", 1.45, 1.0, 5.0),
           createParameter("reflection_loss", "Reflection Loss", "dB", "double", 0.1, 0.0, 100.0)};

    return def;
}

// Если нужно создавать кастомный компонент с произвольными параметрами
inline ComponentDefinition createCustomDefinition(const QString &type,
                                                  const QString &name,
                                                  const QString &category,
                                                  const QString &iconPath,
                                                  const QString &description,
                                                  const QString &simulationModel,
                                                  const QVector<QMap<QString, QVariant>> &pins,
                                                  const QVector<QMap<QString, QVariant>> &parameters)
{
    ComponentDefinition def;
    def.type = type;
    def.name = name;
    def.iconPath = iconPath;
    def.pins = pins;
    def.parameters = parameters;
    return def;
}

} // namespace TestHelpers