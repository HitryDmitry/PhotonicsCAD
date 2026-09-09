#include "Solver.h"
#include "ComponentModelFactory.h"

SimulationResult Solver::solveFrequencyDomain(const SimulationGraph &graph,
                                              const Circuit *circuit,
                                              const std::vector<double> &frequenciesHz)
{
    SimulationResult result;
    result.frequenciesHz = frequenciesHz;

    // Подготавливаем память под результаты
    for (const auto &obsPin : graph.observationPoints) {
        result.frequencyResponses[obsPin].resize(frequenciesHz.size(), 0.0);
    }

    // Считаем схему для каждой частоты отдельно
    for (size_t fIdx = 0; fIdx < frequenciesHz.size(); ++fIdx) {
        double currentFreqHz = frequenciesHz[fIdx];

        // В этой мапе будем накапливать значения сигнала на ВСЕХ пинах схемы в процессе расчета
        std::map<PinRef, double> pinPowers;

        // Идем по компонентам строго в топологическом порядке (от входов к выходам)
        for (const auto &compId : graph.executionOrder) {
            // Используем const_cast, так как findComponent в Circuit не помечен как const
            ComponentInstance *comp = const_cast<Circuit *>(circuit)->findComponent(compId);
            if (!comp)
                continue;

            auto model = ComponentModelFactory::create(comp);

            // Формируем массив входных мощностей на каждом пине [1 x N], где N - общее количество пинов
            std::vector<double> inputPowersVec(comp->getNumPins(), 0.0);
            bool isSource = true;

            // 1. Собираем мощности со всех входов компонента,
            // считаем, что на выходных пинах приходящая мощность нулевая
            for (const auto &pinPtr : comp->mPins) {
                std::string dir = pinPtr->getDirection();
                if (dir == "input" || dir == "in" || dir == "INPUT") {
                    isSource = false;
                    auto currentPinIdx = pinPtr->getPinIdx();
                    PinRef currentInputPin{compId, currentPinIdx};

                    // Смотрим в граф: какой выходной пин подключен к нашему входу?
                    auto it = graph.inputToOutput.find(currentInputPin);
                    if (it != graph.inputToOutput.end()) {
                        PinRef sourceOutPin = it->second;
                        inputPowersVec[currentPinIdx.value()] = pinPowers[sourceOutPin];
                    }
                }
            }

            // Если входов нет, значит это источник (лазер). Сигнал зарождается здесь.
            if (isSource) {
                inputPowersVec[0] = 1.0;
            }

            // Преобразуем вектор входных мощностей в матрицу
            Matrix inputPowerMtx(inputPowersVec);

            // 2. Умножаем матрицу мощностных S-параметров на столбец входных мощностей
            // Например, для делителя размер результата:
            // [3 x 3] * [3 x 1] = [3 x 1] - столбец выходных мощностей
            Matrix outputPowersMtx = model->transferFunction(currentFreqHz)
                                     * inputPowerMtx.transpose();

            // 3. Отправляем результат на все ВЫХОДЫ компонента
            for (const auto &pinPtr : comp->mPins) {
                std::string dir = pinPtr->getDirection();
                if (dir == "output" || dir == "out" || dir == "OUTPUT") {
                    auto pinIdx = pinPtr->getPinIdx();
                    PinRef currentOutputPin{compId, pinIdx};

                    // Индексу выходного пина соответствует индекс в столбце выходных мощностей
                    pinPowers[currentOutputPin] = outputPowersMtx(pinIdx.value(), 0);
                }
            }
        }

        // 4. После того как посчитали всю схему на данной частоте,
        // сохраняем результаты для точек наблюдения
        for (const auto &obsPin : graph.observationPoints) {
            auto it = graph.inputToOutput.find(obsPin);
            if (it != graph.inputToOutput.end()) {
                result.frequencyResponses[obsPin][fIdx] = pinPowers[it->second];
            }
        }
    }

    return result;
}