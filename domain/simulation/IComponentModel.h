#pragma once
#include "Matrix.h"

class IComponentModel {
public:
    virtual ~IComponentModel() = default;

    // Передаточная функция (Transfer Function) для частотной области
    // Принимает частоту и возвращает комплексный коэффициент изменения сигнала
    virtual Matrix transferFunction(double frequency) = 0;
};