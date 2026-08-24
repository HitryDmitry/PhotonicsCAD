#pragma once
#include <vector>
#include <complex>
#include <map>
#include "SimulationGraph.h"
#include "Circuit.h"

// Структура, хранящая результаты вычислений
struct SimulationResult {
    std::vector<double> frequencies; // Массив частот, на которых считали
    // Для каждого пина (точки наблюдения) хранится массив значений сигнала
    std::map<PinRef, std::vector<std::complex<double>>> frequencyResponses;
};

class Solver {
public:
    // Метод расчета схемы в частотной области
    SimulationResult solveFrequencyDomain(
        const SimulationGraph& graph,
        const Circuit* circuit,
        const std::vector<double>& frequencies
        );
};