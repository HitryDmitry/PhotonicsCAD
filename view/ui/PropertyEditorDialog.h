#pragma once
#include <QDialog>
#include <QMap>

class ComponentViewModel;
class ComponentDefinition;
class QFormLayout;

class PropertyEditorDialog : public QDialog
{
    Q_OBJECT

public:
    PropertyEditorDialog(ComponentViewModel *compVM,
                         const ComponentDefinition *def,
                         QWidget *parent = nullptr);

private:
    ComponentViewModel *mCompVM;
    const ComponentDefinition *definition;

    QMap<QString, QWidget *> editors;

    void buildUI();
    void applyChanges();
};
