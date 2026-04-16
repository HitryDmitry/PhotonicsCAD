#pragma once
#include <QVector>
#include "ComponentDefinition.h"

class ComponentLibraryManager
{
public:
    bool loadFromJson(const QString &path);
    QVector<ComponentDefinition> getComponents() const;
    const ComponentDefinition *getByType(const QString &type) const;

private:
    QVector<ComponentDefinition> components;
};
