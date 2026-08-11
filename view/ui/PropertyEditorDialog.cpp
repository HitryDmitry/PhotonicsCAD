#include "PropertyEditorDialog.h"
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include "ComponentDefinition.h"
#include "ComponentViewModel.h"
#include <string>

PropertyEditorDialog::PropertyEditorDialog(ComponentViewModel *vm,
                                           const ComponentDefinition *def,
                                           QWidget *parent)
    : QDialog(parent)
    , mCompVM(vm)
    , definition(def)
{
    setWindowTitle(def->name);
    mCompVM->addObserver(this);
    buildUI();
}

PropertyEditorDialog::~PropertyEditorDialog()
{
    if (mCompVM) {
        mCompVM->removeObserver(this);
    }
}

void PropertyEditorDialog::onPropertyModyfied()
{
    qDebug() << "Parameters were successfully changed.";
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

        QVariant value = paramDef["default"];

        // Преобразуем ключ в std::string для поиска в стандартной карте
        std::string stdKey = key.toStdString();

        for (const auto &paramInst : mCompVM->getInstanceParamsVector()) {
            bool containsValue = false;
            for (const auto &[k, v] : paramInst) {
                if (v == stdKey) {
                    containsValue = true;
                    break;
                }
            }

            if (containsValue) {
                // Безопасный поиск значения "default" в константной std::map
                auto it = paramInst.find("default");
                if (it != paramInst.end()) {
                    value = QString::fromStdString(it->second);
                }
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
        std::string stdKey = key.toStdString();

        for (auto &paramInst : mCompVM->getInstanceParamsVector()) {
            bool containsValue = false;
            for (const auto &[k, v] : paramInst) {
                if (v == stdKey) {
                    containsValue = true;
                    break;
                }
            }

            if (containsValue) {
                // Конвертируем числовые и строковые типы обратно в std::string
                if (datatype == "double") {
                    double val = static_cast<QDoubleSpinBox *>(editor)->value();
                    mCompVM->modifyProperty(stdKey, QString::number(val).toStdString());

                } else if (datatype == "int") {
                    int val = static_cast<QSpinBox *>(editor)->value();
                    mCompVM->modifyProperty(stdKey, std::to_string(val));

                } else {
                    QString val = static_cast<QLineEdit *>(editor)->text();
                    mCompVM->modifyProperty(stdKey, val.toStdString());
                }
            }
        }
    }
}