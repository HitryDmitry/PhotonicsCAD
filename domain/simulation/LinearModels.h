#pragma once
#include "IComponentModel.h"

class LaserModel : public IComponentModel {
public:
    std::complex<double> transferFunction(double frequency) override { return {1.0, 0.0}; }
};

class FiberModel : public IComponentModel {
public:
    std::complex<double> transferFunction(double frequency) override { return {0.9, 0.0}; }
};