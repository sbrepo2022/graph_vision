#ifndef FORMGENERATOR_H
#define FORMGENERATOR_H

#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>

#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QToolButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>
#include <QProgressDialog>

#include <QList>
#include <QMutableListIterator>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QImage>

#include <QDebug>

class FormGenerator : public QObject {
    Q_OBJECT

public:
    explicit FormGenerator(QObject *parent = nullptr);

    void generateWidget(QWidget *parent_widget, QFormLayout *layout, const QList<QMap<QString, QVariant>> &widget_properties);

    virtual ~FormGenerator();
};

#endif // FORMGENERATOR_H
