#include "ComponentInstance.h"

ComponentInstance::ComponentInstance(const ComponentDefinition &def)
{
    // Преобразование QString из ComponentDefinition в std::string
    type = def.type.toStdString();

    // Преобразование структуры параметров Qt во внутреннюю стандартную структуру
    for (qsizetype i = 0; i < def.parameters.count(); i++) {
        std::map<std::string, std::string> params;
        for (const auto &[key, value] : def.parameters[i].asKeyValueRange()) {
            params[key.toStdString()] = value.toString().toStdString();
        }
        parameters.push_back(params);
    }
}