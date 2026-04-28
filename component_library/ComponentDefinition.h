#pragma once
#include <QMap>
#include <QString>
#include <QVariant>

struct ComponentDefinition
{
    QString type;
    QString name;
    QString iconPath;
    QVector<QMap<QString, QVariant>> pins;
    QVector<QMap<QString, QVariant>> parameters;
};
