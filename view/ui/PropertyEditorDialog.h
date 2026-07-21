#pragma once
#include <QDialog>
#include <QMap>

class ComponentInstance;
class ComponentDefinition;
class QFormLayout;

class PropertyEditorDialog : public QDialog
{
    Q_OBJECT

public:
    PropertyEditorDialog(ComponentInstance *instance,
                         const ComponentDefinition *def,
                         QWidget *parent = nullptr);

private:
    ComponentInstance *instance;
    const ComponentDefinition *definition;

    QMap<QString, QWidget *> editors;

    void buildUI();
    void applyChanges();
};
