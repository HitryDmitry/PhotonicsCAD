#pragma once

#include <map>
#include <memory>
#include <string>

#include "Circuit.h"
#include "ComponentDefinition.h"
#include "ComponentId.h"
#include "ComponentInstance.h"
#include "PinIndex.h"

// Forward declaration of TestHelpers (если нужно)
namespace TestHelpers {
ComponentId makeComponentId(uint64_t id);
PinIndex makePinIndex(uint16_t index);
ComponentDefinition createLaserDefinition();
ComponentDefinition createOpticalFiberDefinition();
ComponentDefinition createElectroOpticModulatorDefinition();
ComponentDefinition createPhotodetectorDefinition();
ComponentDefinition createMicrowaveAmplifierDefinition();
QMap<QString, QVariant> createPin(const QString &id,
                                  const QString &name,
                                  const QString &direction,
                                  const QString &signalType);
QMap<QString, QVariant> createParameter(const QString &key,
                                        const QString &name,
                                        const QString &unit,
                                        const QString &datatype,
                                        double defaultValue,
                                        double minValue,
                                        double maxValue);
} // namespace TestHelpers

/**
 * @brief Класс для построения тестовых схем
 * 
 * Предоставляет удобный интерфейс для создания схем с различными
 * компонентами и соединениями для целей тестирования.
 */
class CircuitBuilder
{
public:
    CircuitBuilder();
    ~CircuitBuilder() = default;

    // Запрещаем копирование
    CircuitBuilder(const CircuitBuilder &) = delete;
    CircuitBuilder &operator=(const CircuitBuilder &) = delete;

    // Разрешаем перемещение
    CircuitBuilder(CircuitBuilder &&) = default;
    CircuitBuilder &operator=(CircuitBuilder &&) = default;

    /**
     * @brief Регистрация типа компонента в библиотеке
     * @param def Определение компонента
     */
    void registerComponentType(const ComponentDefinition &def);

    /**
     * @brief Добавление компонента в схему
     * @param type Тип компонента (должен быть зарегистрирован)
     * @param params Параметры компонента (имя -> значение)
     * @return ID созданного компонента
     * @throws std::runtime_error если тип неизвестен или не удалось добавить
     */
    ComponentId addComponent(const std::string &type,
                             const std::map<std::string, std::string> &params = {});

    /**
     * @brief Добавление провода между компонентами
     * @param from ID компонента-источника
     * @param fromPin Имя пина-источника
     * @param to ID компонента-приемника
     * @param toPin Имя пина-приемника
     * @throws std::runtime_error если не удалось добавить провод
     */
    void addWire(ComponentId from,
                 const std::string &fromPin,
                 ComponentId to,
                 const std::string &toPin);

    /**
     * @brief Завершение построения схемы
     * @return unique_ptr на готовую схему
     */
    std::unique_ptr<Circuit> build();

    /**
     * @brief Получение ссылки на схему (для отладки)
     */
    Circuit &getCircuit();
    const Circuit &getCircuit() const;

    /**
     * @brief Поиск компонента по ID
     * @param id ID компонента
     * @return Указатель на компонент или nullptr
     */
    ComponentInstance *findComponent(ComponentId id);

    /**
     * @brief Получение определения компонента по ID
     * @param id ID компонента
     * @return Указатель на определение или nullptr
     */
    const ComponentDefinition *getDefinition(ComponentId id);

    /**
     * @brief Получение индекса пина по ID компонента и текстовому id пина
     * @param id ID компонента
     * @param pinId ID пина (текстовое)
     * @return Указатель на определение или nullptr
     */
    PinIndex getPinIndex(ComponentId id, const std::string &pinId);

private:
    // Приватные методы для создания определений компонентов
    ComponentDefinition createSplitterDefinition();
    ComponentDefinition createTerminalDefinition();
    ComponentDefinition createCombinerDefinition();

    // Вспомогательные методы для работы с пинами
    void cachePinIndices(ComponentId id, const ComponentDefinition &def);

    // Данные
    Circuit mCircuit;
    std::map<std::string, ComponentDefinition> mDefinitions;               // тип -> определение
    std::map<ComponentId, std::map<std::string, PinIndex>> mPinIndexCache; // ID -> {имя пина -> индекс}
    ComponentId mNextId;
};