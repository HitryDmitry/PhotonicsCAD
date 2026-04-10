#pragma once
#include <QVector>
#include "ComponentDefinition.h"

class ComponentLibraryManager
{
public:
    bool loadFromJson(const QString &path);
    QVector<ComponentDefinition> getComponents() const;

private:
    QVector<ComponentDefinition> components;
};
