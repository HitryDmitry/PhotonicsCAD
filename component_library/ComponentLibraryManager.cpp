#include "ComponentLibraryManager.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

bool ComponentLibraryManager::loadFromJson(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray arr = doc.array();

    for (auto item : arr) {
        QJsonObject obj = item.toObject();

        ComponentDefinition comp;
        comp.type = obj["type"].toString();
        comp.name = obj["name"].toString();
        comp.iconPath = obj["icon"].toString();
        comp.pins = obj["pins"].toInt();

        QJsonObject params = obj["parameters"].toObject();
        for (auto key : params.keys())
            comp.parameters[key] = params[key].toString();

        components.push_back(comp);
    }

    return true;
}

QVector<ComponentDefinition> ComponentLibraryManager::getComponents() const
{
    return components;
}
