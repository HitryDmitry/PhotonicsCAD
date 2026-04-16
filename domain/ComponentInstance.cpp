#include "ComponentInstance.h"
#include <QMap>

ComponentInstance::ComponentInstance(const ComponentDefinition &def)
{
    type = def.type;
    for (auto [key, value] : def.parameters.asKeyValueRange()) {
        parameters.insert(key, value);
    }
}
