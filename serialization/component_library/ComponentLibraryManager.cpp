#include "ComponentLibraryManager.h"
#include <QDebug>
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

        QJsonArray pins = obj["pins"].toArray();
        for (auto pinIter : pins) {
            QMap<QString, QVariant> pin;
            pin["id"] = pinIter.toObject()["id"].toString();
            pin["name"] = pinIter.toObject()["name"].toString();
            pin["direction"] = pinIter.toObject()["direction"].toString();
            pin["signal_type"] = pinIter.toObject()["signal_type"].toString();

            comp.pins.append(pin);
        }

        QJsonArray params = obj["parameters"].toArray();
        for (auto param : params) {
            QMap<QString, QVariant> parameter;
            parameter["key"] = param.toObject()["key"].toString();
            parameter["name"] = param.toObject()["name"].toString();
            parameter["unit"] = param.toObject()["unit"].toString();
            parameter["datatype"] = param.toObject()["datatype"].toString();
            parameter["default"] = param.toObject()["default"].toDouble();
            parameter["min"] = param.toObject()["min"].toDouble();
            parameter["max"] = param.toObject()["max"].toDouble();

            comp.parameters.append(parameter);
        }

        components.push_back(comp);
    }

    return true;
}

QVector<ComponentDefinition> ComponentLibraryManager::getComponents() const
{
    return components;
}

const ComponentDefinition *ComponentLibraryManager::getByType(const QString &type) const
{
    for (const auto &comp : components) {
        if (comp.type == type) {
            return &comp;
        }
    }

    return nullptr;
}
