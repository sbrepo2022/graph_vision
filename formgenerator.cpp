#include "formgenerator.h"

FormGenerator::FormGenerator(QObject *parent) : QObject(parent) {}

void FormGenerator::generateWidget(QWidget *parent_widget, QFormLayout *layout, const QList<QMap<QString, QVariant>> &widget_properties) {
    for (int i = 0; i < widget_properties.size(); i++) {
        QMap<QString, QVariant> prop_elem = widget_properties[i];
        QVariant prop_val = this->property(prop_elem["name"].toString().toStdString().c_str());
        QWidget *field = nullptr;

        // adding property components
        if (strcmp(prop_val.typeName(), "bool") == 0) {
            QCheckBox *check = new QCheckBox(parent_widget);
            check->setChecked(prop_val.toBool());
            connect(check, &QCheckBox::stateChanged, this, [=](bool val) {
                this->setProperty(prop_elem["name"].toString().toStdString().c_str(), val);
            });
            field = check;
        }
        else if (strcmp(prop_val.typeName(), "int") == 0) {
            QSpinBox *spin = new QSpinBox(parent_widget);
            if (prop_elem.find("min") != prop_elem.end())
                spin->setMinimum(prop_elem["min"].toInt());
            if (prop_elem.find("max") != prop_elem.end())
                spin->setMaximum(prop_elem["max"].toInt());
            spin->setValue(prop_val.toInt());
            connect(spin, &QSpinBox::valueChanged, this, [=](int val) {
                this->setProperty(prop_elem["name"].toString().toStdString().c_str(), val);
            });
            field = spin;
        }
        else if (strcmp(prop_val.typeName(), "double") == 0) {
            QDoubleSpinBox *spin = new QDoubleSpinBox(parent_widget);
            if (prop_elem.find("min") != prop_elem.end())
                spin->setMinimum(prop_elem["min"].toDouble());
            if (prop_elem.find("max") != prop_elem.end())
                spin->setMaximum(prop_elem["max"].toDouble());
            spin->setValue(prop_val.toDouble());
            connect(spin, &QDoubleSpinBox::valueChanged, this, [=](double val) {
                this->setProperty(prop_elem["name"].toString().toStdString().c_str(), val);
            });
            field = spin;
        }
        else if (strcmp(prop_val.typeName(), "QString") == 0) {
            if (prop_elem.find("field_type") != prop_elem.end()) {
                if (prop_elem["field_type"].toString() == "list") {
                    QComboBox *combo = new QComboBox(parent_widget);
                    if (prop_elem.find("variants") != prop_elem.end()) {
                        combo->addItems(prop_elem["variants"].toStringList());
                    }
                    connect(combo, &QComboBox::currentTextChanged, this, [=](const QString &val) {
                        this->setProperty(prop_elem["name"].toString().toStdString().c_str(), val);
                    });
                    field = combo;
                }
                else {
                    QLineEdit *line = new QLineEdit(parent_widget);
                    connect(line, &QLineEdit::textChanged, this, [=](const QString &val) {
                        this->setProperty(prop_elem["name"].toString().toStdString().c_str(), val);
                    });
                }
            }
        }
        else {
            field = new QLabel("incorrect property type", parent_widget);
        }

        layout->addRow(new QLabel(prop_elem["name"].toString().toStdString().c_str()), field);
    }
}

FormGenerator::~FormGenerator() {}
