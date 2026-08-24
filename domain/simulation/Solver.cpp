#include "Solver.h"
#include "ComponentModelFactory.h"

SimulationResult Solver::solveFrequencyDomain(
    const SimulationGraph& graph,
    const Circuit* circuit,
    const std::vector<double>& frequencies)
{
    SimulationResult result;
    result.frequencies = frequencies;

    // Подготавливаем память под результаты
    for (const auto& obsPin : graph.observationPoints) {
        result.frequencyResponses[obsPin].resize(frequencies.size(), 0.0);
    }

    // Считаем схему для каждой частоты отдельно
    for (size_t f_idx = 0; f_idx < frequencies.size(); ++f_idx) {
        double currentFreq = frequencies[f_idx];

        // В этой мапе будем накапливать значения сигнала на ВСЕХ пинах схемы в процессе расчета
        // (Используем std::map, т.к. PinRef имеет operator<)
        std::map<PinRef, std::complex<double>> pinSignals;

        // Идем по компонентам строго в топологическом порядке (от входов к выходам)
        for (const auto& compId : graph.executionOrder) {

            // Используем const_cast, так как findComponent в Circuit не помечен как const
            ComponentInstance* comp = const_cast<Circuit*>(circuit)->findComponent(compId);
            if (!comp) continue;

            auto model = ComponentModelFactory::create(comp->getType());

            std::complex<double> inputSignal = 0.0;
            bool isSource = true;

            // 1. Собираем сигнал со всех ВХОДОВ компонента
            for (const auto& pinPtr : comp->mPins) {
                std::string dir = pinPtr->getDirection();
                if (dir == "input" || dir == "in" || dir == "INPUT") {
                    isSource = false;
                    PinRef currentInputPin{compId, pinPtr->getPinIdx()};

                    // Смотрим в граф: какой выходной пин подключен к нашему входу?
                    auto it = graph.inputToOutput.find(currentInputPin);
                    if (it != graph.inputToOutput.end()) {
                        PinRef sourceOutPin = it->second;
                        inputSignal += pinSignals[sourceOutPin]; // Добавляем пришедший сигнал
                    }
                }
            }

            // Если входов нет, значит это источник (лазер). Сигнал зарождается здесь.
            if (isSource) {
                inputSignal = 1.0;
            }

            // 2. Умножаем сигнал на передаточную функцию компонента (математика)
            std::complex<double> outputSignal = inputSignal * model->transferFunction(currentFreq);

            // 3. Отправляем результат на все ВЫХОДЫ компонента
            for (const auto& pinPtr : comp->mPins) {
                std::string dir = pinPtr->getDirection();
                if (dir == "output" || dir == "out" || dir == "OUTPUT") {
                    PinRef currentOutputPin{compId, pinPtr->getPinIdx()};
                    pinSignals[currentOutputPin] = outputSignal;
                }
            }
        }

        // 4. После того как посчитали всю схему на данной частоте,
        // сохраняем результаты для точек наблюдения
        for (const auto& obsPin : graph.observationPoints) {
            auto it = graph.inputToOutput.find(obsPin);
            if (it != graph.inputToOutput.end()) {
                result.frequencyResponses[obsPin][f_idx] = pinSignals[it->second];
            }
        }
    }

    return result;
}