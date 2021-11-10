#ifndef GRAPHPREPROCESS_H
#define GRAPHPREPROCESS_H

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
#include <QLinkedList>
#include <QMutableListIterator>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QImage>

#include <QDebug>

#include <limits>

#include "formgenerator.h"

typedef QLinkedList<QLinkedList<QPoint>> VectorizationProduct;

class GraphPreprocess : public FormGenerator {
    Q_OBJECT;

protected:
    QString group_name;
    QWidget *interface_widget;

    // use generateWidget in constructor of extended class
    // it may generate interface widget for this image processor using its meta properties
    void generateWidget(const QList<QMap<QString, QVariant>> &widget_properties);

public:
    explicit GraphPreprocess(QObject *parent = nullptr);
    QString getGroupName() { return group_name; }
    QWidget* getWidget() { return interface_widget; }
    virtual ~GraphPreprocess();
};



class Vectorization : public GraphPreprocess {
    Q_OBJECT;

public:
    explicit Vectorization(QObject *parent = nullptr) : GraphPreprocess(parent) {};

    virtual VectorizationProduct processData(const QImage &image) = 0;
    virtual ~Vectorization() {};
};



class VectorTransforms : public GraphPreprocess {
    Q_OBJECT;

protected:
    bool use;

    // use generateWidget in constructor of extended class
    // it may generate interface widget for this image processor using its meta properties
    void generateWidget(const QList<QMap<QString, QVariant>> &widget_properties);

public:
    explicit VectorTransforms(QObject *parent = nullptr);
    bool isUse() { return use; }
    virtual VectorizationProduct processData(const VectorizationProduct &vp) = 0;
    virtual ~VectorTransforms();

public slots:
    void useFilter(bool use);
};


// Current filter realizations

class LinearVectorization : public Vectorization {
    Q_OBJECT;
    Q_PROPERTY(int ratio MEMBER m_ratio NOTIFY ratioChanged);

private:
    int m_ratio;

    QLinkedList<QPoint> vectorizeCurve(const QImage &image, bool **used_field, const QPoint &start);

public:
    explicit LinearVectorization(QObject *parent = nullptr);

    virtual VectorizationProduct processData(const QImage &image);
    virtual ~LinearVectorization();

signals:
    void ratioChanged(int);
};



class VectorNoiseClearing : public VectorTransforms {
    Q_OBJECT;
    Q_PROPERTY(int radius MEMBER m_radius NOTIFY radiusChanged);

private:
    int m_radius;

public:
    explicit VectorNoiseClearing(int radius = 10, QObject *parent = nullptr);

    virtual VectorizationProduct processData(const VectorizationProduct &vp);
    virtual ~VectorNoiseClearing();

signals:
    void radiusChanged(int);
};



class VectorMerge : public VectorTransforms {
    Q_OBJECT;
    Q_PROPERTY(int radius MEMBER m_radius NOTIFY radiusChanged);

private:
    int m_radius;

public:
    explicit VectorMerge(int radius = 20, QObject *parent = nullptr);

    virtual VectorizationProduct processData(const VectorizationProduct &vp);
    virtual ~VectorMerge();

signals:
    void radiusChanged(int);
};



// GraphProcessor

class GraphProcessor : public QObject {
    Q_OBJECT;
private:
    QWidget *prop_group_widget;
    Vectorization *vectorization_filter;
    QList<VectorTransforms*> vector_trans_filters;

public:
    explicit GraphProcessor(QWidget *prop_group_widget, QObject *parent = nullptr);
    ~GraphProcessor();

    void setMiddleware(Vectorization *vectorization_filter, const QList<VectorTransforms*> &vector_trans_filters);

public slots:
    void processGraph(const QImage &image);

signals:
    void resultReady(VectorizationProduct);
    void startCalculating(int, QString); // count of filters
    void currentFilter(int, QString); // number, name
    void finishCalculating();
};

#endif // GRAPHPREPROCESS_H
