#include "CircuitBuilder.h"
#include "TestHelpers.h"

#include <stdexcept>
#include <string>

// Конструктор
CircuitBuilder::CircuitBuilder()
    : mNextId(TestHelpers::makeComponentId(1))
{
    // Регистрируем стандартные типы компонентов
    registerComponentType(TestHelpers::createLaserDefinition());
    registerComponentType(TestHelpers::createOpticalFiberDefinition());
    registerComponentType(TestHelpers::createElectroOpticModulatorDefinition());
    registerComponentType(TestHelpers::createPhotodetectorDefinition());
    registerComponentType(TestHelpers::createMicrowaveAmplifierDefinition());

    // Регистрируем дополнительные компоненты для тестов
    registerComponentType(createSplitterDefinition());
    registerComponentType(createTerminalDefinition());
    registerComponentType(createCombinerDefinition());
}

// Регистрация компонента в библиотеке
void CircuitBuilder::registerComponentType(const ComponentDefinition &def)
{
    mDefinitions[def.type.toStdString()] = def;
}

// Добавление компонента в схему
ComponentId CircuitBuilder::addComponent(const std::string &type,
                                         const std::map<std::string, std::string> &params)
{
    auto defIt = mDefinitions.find(type);
    if (defIt == mDefinitions.end()) {
        throw std::runtime_error("Unknown component type: " + type);
    }

    // Создаем экземпляр компонента
    auto instance = std::make_unique<ComponentInstance>(defIt->second, mNextId);

    // Устанавливаем параметры
    for (const auto &[name, value] : params) {
        if (!instance->setParameter(name, value)) {
            throw std::runtime_error("Failed to set parameter: " + name + " = " + value);
        }
    }

    // Добавляем в схему
    if (!mCircuit.addComponent(std::move(instance))) {
        throw std::runtime_error("Failed to add component to circuit");
    }

    // Кэшируем индексы пинов для этого компонента
    cachePinIndices(mNextId, defIt->second);

    return mNextId++;
}

// Добавление провода между компонентами
void CircuitBuilder::addWire(ComponentId from,
                             const std::string &fromPin,
                             ComponentId to,
                             const std::string &toPin)
{
    auto fromIdx = getPinIndex(from, fromPin);
    auto toIdx = getPinIndex(to, toPin);

    PinRef fromRef{from, fromIdx};
    PinRef toRef{to, toIdx};

    if (!mCircuit.addWire(fromRef, toRef)) {
        throw std::runtime_error("Failed to add wire from " + std::to_string(from.value()) + ":"
                                 + fromPin + " to " + std::to_string(to.value()) + ":" + toPin);
    }
}

// Сборка схемы
std::unique_ptr<Circuit> CircuitBuilder::build()
{
    return std::make_unique<Circuit>(std::move(mCircuit));
}

// Получение схемы по ссылке (для отладки)
Circuit &CircuitBuilder::getCircuit()
{
    return mCircuit;
}

const Circuit &CircuitBuilder::getCircuit() const
{
    return mCircuit;
}

// Поиск компонента по ID
ComponentInstance *CircuitBuilder::findComponent(ComponentId id)
{
    return mCircuit.findComponent(id);
}

// Получение определения компонента по ID
const ComponentDefinition *CircuitBuilder::getDefinition(ComponentId id)
{
    // Ищем компонент
    auto *comp = mCircuit.findComponent(id);
    if (!comp) {
        return nullptr;
    }

    // Ищем определение по типу
    auto defIt = mDefinitions.find(comp->getType());
    if (defIt == mDefinitions.end()) {
        return nullptr;
    }

    return &defIt->second;
}

// Создание определения для Splitter
ComponentDefinition CircuitBuilder::createSplitterDefinition()
{
    ComponentDefinition def;
    def.type = "optical_splitter";
    def.name = "Optical Splitter";
    def.iconPath = ":/icons/splitter.png";

    def.pins = {TestHelpers::createPin("in", "Input", "input", "optical"),
                TestHelpers::createPin("out1", "Output 1", "output", "optical"),
                TestHelpers::createPin("out2", "Output 2", "output", "optical")};

    def.parameters = {
        TestHelpers::createParameter("split_ratio", "Split Ratio", "%", "double", 50.0, 0.0, 100.0),
        TestHelpers::createParameter("excess_loss", "Excess Loss", "dB", "double", 0.5, 0.0, 100.0)};

    return def;
}

// Создание определения для Terminal
ComponentDefinition CircuitBuilder::createTerminalDefinition()
{
    ComponentDefinition def;
    def.type = "terminal";
    def.name = "Terminal";
    def.iconPath = ":/icons/terminal.png";

    def.pins = {TestHelpers::createPin("in", "Input", "input", "optical")};

    def.parameters = {TestHelpers::createParameter(
        "load_impedance", "Load Impedance", "Ohm", "double", 50.0, 0.0, 1000000.0)};

    return def;
}

// Создание определения для Combiner
ComponentDefinition CircuitBuilder::createCombinerDefinition()
{
    ComponentDefinition def;
    def.type = "optical_combiner";
    def.name = "Optical Combiner";
    def.iconPath = ":/icons/combiner.png";

    def.pins = {TestHelpers::createPin("in1", "Input 1", "input", "optical"),
                TestHelpers::createPin("in2", "Input 2", "input", "optical"),
                TestHelpers::createPin("out", "Output", "output", "optical")};

    def.parameters = {TestHelpers::createParameter(
        "combining_loss", "Combining Loss", "dB", "double", 0.5, 0.0, 100.0)};

    return def;
}

// Кэширование индексов пинов для компонента
void CircuitBuilder::cachePinIndices(ComponentId id, const ComponentDefinition &def)
{
    std::map<std::string, PinIndex> indexMap;
    for (size_t i = 0; i < def.pins.size(); ++i) {
        QString pinId = def.pins[i]["id"].toString();
        auto pinIdx = TestHelpers::makePinIndex(static_cast<std::uint16_t>(i));
        indexMap.emplace(pinId.toStdString(), pinIdx);
    }
    mPinIndexCache[id] = indexMap;
}

// Получение индекса пина по имени
PinIndex CircuitBuilder::getPinIndex(ComponentId id, const std::string &pinId)
{
    auto cacheIt = mPinIndexCache.find(id);
    if (cacheIt == mPinIndexCache.end()) {
        // Если нет в кэше, пробуем найти через ComponentInstance
        auto *comp = mCircuit.findComponent(id);
        if (!comp) {
            throw std::runtime_error("Component not found: " + std::to_string(id.value()));
        }

        // Ищем определение по типу
        auto defIt = mDefinitions.find(comp->getType());
        if (defIt == mDefinitions.end()) {
            throw std::runtime_error("Definition not found for type: " + comp->getType());
        }

        // Строим кэш
        cachePinIndices(id, defIt->second);
        cacheIt = mPinIndexCache.find(id);
    }

    auto pinIt = cacheIt->second.find(pinId);
    if (pinIt == cacheIt->second.end()) {
        throw std::runtime_error("Pin not found: " + pinId + " in component "
                                 + std::to_string(id.value()));
    }

    return pinIt->second;
}