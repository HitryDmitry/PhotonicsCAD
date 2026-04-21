#pragma once
#include <QMap>
#include <QString>
#include <QVariant>

struct ComponentDefinition
{
    QString type;
    QString name;
    QString iconPath;
    int pins;
    QVector<QMap<QString, QVariant>> parameters;
};
