#pragma once
#include "ComponentDefinition.h"

namespace TestHelpers {
ComponentDefinition createLaserDefinition()
{
    ComponentDefinition def;
    def.type = "laser";
    def.name = "Laser Source";
    def.iconPath = ":/icons/laser.png";

    // Пины
    QMap<QString, QVariant> inputPin;
    inputPin["name"] = "input";
    inputPin["type"] = "optical";
    inputPin["direction"] = "in";

    QMap<QString, QVariant> outputPin;
    outputPin["name"] = "output";
    outputPin["type"] = "optical";
    outputPin["direction"] = "out";

    def.pins = {inputPin, outputPin};

    // Параметры
    QMap<QString, QVariant> wavelengthParam;
    wavelengthParam["name"] = "wavelength";
    wavelengthParam["value"] = 1550.0;
    wavelengthParam["unit"] = "nm";

    def.parameters = {wavelengthParam};

    return def;
}

ComponentDefinition createSplitterDefinition()
{
    ComponentDefinition def;
    def.type = "splitter";
    def.name = "Beam Splitter";
    def.iconPath = ":/icons/splitter.png";

    QMap<QString, QVariant> inputPin;
    inputPin["name"] = "input";
    inputPin["type"] = "optical";
    inputPin["direction"] = "in";

    QMap<QString, QVariant> outputPin1;
    outputPin1["name"] = "output1";
    outputPin1["type"] = "optical";
    outputPin1["direction"] = "out";

    QMap<QString, QVariant> outputPin2;
    outputPin2["name"] = "output2";
    outputPin2["type"] = "optical";
    outputPin2["direction"] = "out";

    def.pins = {inputPin, outputPin1, outputPin2};

    QMap<QString, QVariant> ratioParam;
    ratioParam["name"] = "split_ratio";
    ratioParam["value"] = 0.5;

    def.parameters = {ratioParam};

    return def;
}
} // namespace TestHelpers