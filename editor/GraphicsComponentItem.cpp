#include "GraphicsComponentItem.h"
#include <QPixmap>

GraphicsComponentItem::GraphicsComponentItem(ComponentDefinition &def)
{
    QString iconPath = ":/icons/" + def.type + ".png";
    setPixmap(QPixmap(iconPath));

    setFlags(ItemIsMovable | ItemIsSelectable);
}
