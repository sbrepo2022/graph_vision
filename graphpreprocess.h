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
#include <QStack>
#include <QMutableListIterator>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QImage>

#include <QDebug>

#include <limits>

#include "formgenerator.h"

class GraphPoint {
private:
    QPoint m_point;
    QList<GraphPoint*> next_points;

public:
    GraphPoint() : m_point(0, 0) {}
    GraphPoint(const QPoint &point) : m_point(point) {}

    QPoint point() { return m_point; }
    void setPoint(const QPoint &point) { m_point = point; }
    void addNext(GraphPoint *next) { next_points.append(next); }
    QList<GraphPoint*> getNext() { return next_points; }

    ~GraphPoint() {
        for (GraphPoint* gp : next_points) {
            delete gp;
        }
    }
};

typedef QLinkedList<QLinkedList<QPoint>> VectorizationProduct;
typedef QList<GraphPoint*> VectorizationProductGraph;

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




// Graph vectorization

class VectorizationGraph : public GraphPreprocess {
    Q_OBJECT;

public:
    explicit VectorizationGraph(QObject *parent = nullptr) : GraphPreprocess(parent) {};

    virtual VectorizationProductGraph processData(const QImage &image) = 0;
    virtual ~VectorizationGraph() {};
};



class LinearVectorizationGraph : public VectorizationGraph {
    Q_OBJECT;
    Q_PROPERTY(int square_size MEMBER m_square_size NOTIFY squareSizeChanged);

private:
    int m_square_size;

    GraphPoint* vectorizeCurve(const QImage &image, bool **used_field, const QPoint &start);

public:
    explicit LinearVectorizationGraph(QObject *parent = nullptr);

    virtual VectorizationProductGraph processData(const QImage &image);
    virtual ~LinearVectorizationGraph();

signals:
    void squareSizeChanged(int);
};

// GraphProcessorGraph

class GraphProcessorGraph : public QObject {
    Q_OBJECT;
private:
    QWidget *prop_group_widget;
    VectorizationGraph *vectorization_filter;

public:
    explicit GraphProcessorGraph(QWidget *prop_group_widget, QObject *parent = nullptr);
    ~GraphProcessorGraph();

    void setMiddleware(VectorizationGraph *vectorization_filter);

public slots:
    void processGraph(const QImage &image);

signals:
    void resultReady(VectorizationProductGraph);
    void startCalculating(int, QString); // count of filters
    void currentFilter(int, QString); // number, name
    void finishCalculating();
};



#endif // GRAPHPREPROCESS_H
