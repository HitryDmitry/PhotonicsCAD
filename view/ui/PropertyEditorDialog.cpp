#include "PropertyEditorDialog.h"
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include "ComponentDefinition.h"
#include "ComponentInstance.h"

PropertyEditorDialog::PropertyEditorDialog(ComponentInstance *instance,
                                           const ComponentDefinition *def,
                                           QWidget *parent)
    : QDialog(parent)
    , instance(instance)
    , definition(def)
{
    setWindowTitle(def->name);
    buildUI();
}

void PropertyEditorDialog::buildUI()
{
    auto layout = new QVBoxLayout(this);
    auto form = new QFormLayout();

    for (const auto &paramDef : definition->parameters) {
        QString key = paramDef["key"].toString();
        QString name = paramDef["name"].toString();
        QString unit = paramDef["unit"].toString();
        QString datatype = paramDef["datatype"].toString();

        // для поиска соответствующего параметра в instance пока используется такое решение
        QVariant value = paramDef["default"];
        for (const auto &paramInst : instance->parameters) {
            if (paramInst.values().contains(key)) {
                value = paramInst["default"];
            }
        }

        QWidget *editor = nullptr;

        if (datatype == "double") {
            auto spin = new QDoubleSpinBox();
            spin->setRange(paramDef["min"].toDouble(), paramDef["max"].toDouble());
            spin->setValue(value.toDouble());
            editor = spin;

        } else if (datatype == "int") {
            auto spin = new QSpinBox();
            spin->setRange(paramDef["min"].toInt(), paramDef["max"].toInt());
            spin->setValue(value.toInt());
            editor = spin;

        } else {
            auto edit = new QLineEdit(value.toString());
            editor = edit;
        }

        editors[key] = editor;

        form->addRow(name + " (" + unit + ")", editor);
    }

    layout->addLayout(form);

    // кнопки
    auto btnLayout = new QHBoxLayout();

    auto okBtn = new QPushButton("OK");
    auto cancelBtn = new QPushButton("Cancel");

    connect(okBtn, &QPushButton::clicked, this, [this]() {
        applyChanges();
        accept();
    });

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    layout->addLayout(btnLayout);
}

void PropertyEditorDialog::applyChanges()
{
    for (auto &paramDef : definition->parameters) {
        QString key = paramDef["key"].toString();
        QString datatype = paramDef["datatype"].toString();

        QWidget *editor = editors[key];

        for (auto &paramInst : instance->parameters) {
            if (paramInst.values().contains(key)) {
                if (datatype == "double") {
                    paramInst["default"] = static_cast<QDoubleSpinBox *>(editor)->value();

                } else if (datatype == "int") {
                    paramInst["default"] = static_cast<QSpinBox *>(editor)->value();

                } else {
                    paramInst["default"] = static_cast<QLineEdit *>(editor)->text();
                }
            }
        }
    }
}
