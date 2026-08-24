#pragma once
#include <complex>

class IComponentModel {
public:
    virtual ~IComponentModel() = default;

    // Передаточная функция (Transfer Function) для частотной области
    // Принимает частоту и возвращает комплексный коэффициент изменения сигнала
    virtual std::complex<double> transferFunction(double frequency) = 0;
};