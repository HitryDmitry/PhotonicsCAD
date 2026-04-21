#include "ComponentInstance.h"
#include <QMap>

ComponentInstance::ComponentInstance(const ComponentDefinition &def)
{
    type = def.type;
    for (qsizetype i = 0; i < def.parameters.count(); i++) {
        QMap<QString, QVariant> params;
        for (const auto &[key, value] : def.parameters[i].asKeyValueRange()) {
            params.insert(key, value);
        }
        parameters.push_back(params);
    }
}
