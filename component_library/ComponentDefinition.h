#pragma once
#include <QMap>
#include <QString>

struct ComponentDefinition
{
    QString type;
    QString name;
    QString iconPath;
    int pins;
    QMap<QString, QString> parameters;
};
