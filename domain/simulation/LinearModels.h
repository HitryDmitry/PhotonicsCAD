#pragma once
#include "IComponentModel.h"

// Модель источника (Лазер)
class LaserModel : public IComponentModel {
public:
    std::complex<double> transferFunction(double frequency) override {
        return {1.0, 0.0}; // Выдает идеальный нормированный сигнал
    }
};

// Модель простого соединителя / волокна
class FiberModel : public IComponentModel {
public:
    std::complex<double> transferFunction(double frequency) override {
        return {0.9, 0.0}; // Пропускает сигнал, ослабляя его на 10% (коэффициент 0.9)
    }
};