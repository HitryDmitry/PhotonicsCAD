#pragma once
#include <QGraphicsPixmapItem>

#include "ComponentDefinition.h"

class GraphicsComponentItem : public QGraphicsPixmapItem
{
public:
    GraphicsComponentItem(ComponentDefinition &def);
};
