#pragma once
#include <QDialog>
#include <QMap>
#include "ComponentViewModel.h"

class ComponentViewModel;
class ComponentDefinition;
class QFormLayout;

class PropertyEditorDialog : public QDialog, public IComponentObserver
{
    Q_OBJECT

public:
    PropertyEditorDialog(ComponentViewModel *compVM,
                         const ComponentDefinition *def,
                         QWidget *parent = nullptr);
    ~PropertyEditorDialog() override;
    void onPropertyModyfied() override;

private:
    ComponentViewModel *mCompVM;
    const ComponentDefinition *definition;

    QMap<QString, QWidget *> editors;

    void buildUI();
    void applyChanges();
};
