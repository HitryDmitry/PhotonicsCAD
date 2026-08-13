#include "ComponentInstance.h"

ComponentInstance::ComponentInstance(const ComponentDefinition &def)
{
    // Тип компонента
    mType = def.type.toStdString();
    size_t paramCount = def.parameters.count();
    // Преобразуем структуру параметров Qt во внутреннюю стандартную структуру, и передаем компоненту
    for (size_t i = 0; i < paramCount; i++) {
        std::map<std::string, std::string> params;
        for (const auto &[key, value] : def.parameters[i].asKeyValueRange()) {
            params[key.toStdString()] = value.toString().toStdString();
        }
        mParameters.push_back(params);
    }

    // Создаем пины компонента
    size_t pinCount = def.pins.size();

    for (size_t i = 0; i < pinCount; i++) {
        // Ссылка на словарь, описывающий пин: def->pins.at(i)
        // Возможно это понадобится при определении положения пинов в зависимости от
        // параметров пина
        const auto &currentPin = def.pins.at(i);

        // Конвертируем QMap<QString, QVariant> в std::map<std::string, std::string>
        std::map<std::string, std::string> stdPinDef;
        for (const auto &[key, value] : currentPin.asKeyValueRange()) {
            stdPinDef[key.toStdString()] = value.toString().toStdString();
        }

        auto pinInst = std::make_unique<PinInstance>(stdPinDef);
        pinInst->component = this;
        mPins.push_back(std::move(pinInst));
    }
}

bool ComponentInstance::setParameter(const std::string &name, const std::string &value)
{
    for (auto &paramInst : mParameters) {
        if (paramInst.at("key") == name) {
            auto it = paramInst.find("default");
            if (it != paramInst.end()) {
                it->second = value;
                return 1;
            }
        }
    }
    return 0;
}

double ComponentInstance::getX() const
{
    return mPosition.x;
}
double ComponentInstance::getY() const
{
    return mPosition.y;
}