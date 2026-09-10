#pragma once
#include "ComponentInstance.h"
#include "IComponentModel.h"
#include "LinearModels.h"
#include <memory>
#include <string>

class ComponentModelFactory {
public:
    static std::unique_ptr<IComponentModel> create(ComponentInstance *instance)
    {
        const std::string &type = instance->getType();
        int numPins = instance->getNumPins();

        if (type == "laser") {
            double power = std::stod(instance->getParameter("power"));
            double freq = std::stod(instance->getParameter("frequency"));
            double RIN = std::stod(instance->getParameter("rin_noise"));

            return std::make_unique<LaserModel>(power, freq, RIN, numPins);
        }

        else if (type == "optical_fiber") {
            double length = std::stod(instance->getParameter("length"));
            double attenuation = std::stod(instance->getParameter("attenuation"));
            double n = std::stod(instance->getParameter("refractive_index"));
            return std::make_unique<FiberModel>(length, attenuation, n, numPins);
        }

        else if (type == "optical_splitter") {
            double ratio = std::stod(instance->getParameter("split_ratio"));
            return std::make_unique<SplitterModel>(ratio, numPins);
        }

        else if (type == "photodetector") {
            double responsivityAmpPerWatt = std::stod(instance->getParameter("responsivity"));
            double maxCurrentMilliamperes = std::stod(instance->getParameter("max_current"));
            double loadResistOhm = std::stod(instance->getParameter("load_resistance"));

            return std::make_unique<Photodetector>(responsivityAmpPerWatt,
                                                   maxCurrentMilliamperes * Units::mA,
                                                   loadResistOhm,
                                                   numPins);
        }

        return std::make_unique<FiberModel>(1.0, 0.0, 1.0, numPins);
    }
};