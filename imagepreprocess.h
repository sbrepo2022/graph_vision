#ifndef IMAGEPREPROCESS_H
#define IMAGEPREPROCESS_H

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
#include <QStack>
#include <QMutableListIterator>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QImage>

#include <QDebug>

#include <algorithm>

#include "formgenerator.h"
#include "algorithms.h"

class ImagePreprocess : public FormGenerator {
    Q_OBJECT;

protected:
    QWidget *interface_widget;
    QString group_name;
    bool use;

    // use generateWidget in constructor of extended class
    // it may generate interface widget for this image processor using its meta properties
    void generateWidget(const QList<QMap<QString, QVariant>> &widget_properties);

public:
    explicit ImagePreprocess(QObject *parent = nullptr);
    bool isUse() { return use; }
    QString getGroupName() { return group_name; }
    QWidget* getWidget() { return interface_widget; }

    virtual QImage processImage(const QImage &image) = 0;
    virtual ~ImagePreprocess();

public slots:
    void useFilter(bool use);

signals:
    void moveUpPreprocess();
    void moveDownPreprocess();
    void deletePreprocess();
};

// image processors implementations

class MonochromeGradientImage : public ImagePreprocess {
    Q_OBJECT;
    Q_PROPERTY(int threshold MEMBER m_threshold NOTIFY thresholdChanged);

private:
    int m_threshold;

public:
    explicit MonochromeGradientImage(QObject *parent = nullptr);

    virtual QImage processImage(const QImage &image);
    virtual ~MonochromeGradientImage();

signals:
    void thresholdChanged(int);
};



class GaussianBlur : public ImagePreprocess {
    Q_OBJECT;
    Q_PROPERTY(double sigma MEMBER m_sigma NOTIFY sigmaChanged);
    Q_PROPERTY(int size MEMBER m_size NOTIFY sizeChanged);

private:
    double m_sigma;
    int m_size;

public:
    explicit GaussianBlur(QObject *parent = nullptr);

    virtual QImage processImage(const QImage &image);
    virtual ~GaussianBlur();

signals:
    void sigmaChanged(double);
    void sizeChanged(int);
};

class CannyFilter : public ImagePreprocess {
    Q_OBJECT;
    Q_PROPERTY(QString filter MEMBER m_filter NOTIFY filterChanged);
    Q_PROPERTY(bool supression MEMBER m_supression NOTIFY supressionChanged);
    Q_PROPERTY(int threshold_low MEMBER m_threshold_low NOTIFY thresholdLowChanged);
    Q_PROPERTY(int threshold_high MEMBER m_threshold_high NOTIFY thresholdHighChanged);
    Q_PROPERTY(bool hysteresis MEMBER m_hysteresis NOTIFY hysteresisChanged);

private:
    QString m_filter;
    bool m_supression;
    int m_threshold_low;
    int m_threshold_high;
    bool m_hysteresis;

public:
    explicit CannyFilter(QObject *parent = nullptr);

    virtual QImage processImage(const QImage &image);
    virtual ~CannyFilter();

signals:
    void filterChanged(QString);
    void supressionChanged(bool);
    void thresholdLowChanged(int);
    void thresholdHighChanged(int);
    void hysteresisChanged(bool);
};

class ColorGradientField : public ImagePreprocess {
    Q_OBJECT;
    Q_PROPERTY(QString filter MEMBER m_filter NOTIFY filterChanged);
    Q_PROPERTY(bool grayscale MEMBER m_grayscale NOTIFY grayscaleChanged);

private:
    QString m_filter;
    bool m_grayscale;

public:
    explicit ColorGradientField(QObject *parent = nullptr);

    virtual QImage processImage(const QImage &image);
    virtual ~ColorGradientField();

signals:
    void filterChanged(QString);
    void grayscaleChanged(bool);
};

class SegmentationField : public ImagePreprocess {
    Q_OBJECT;
    Q_PROPERTY(int threshold_low MEMBER m_threshold_low NOTIFY thresholdLowChanged);
    Q_PROPERTY(bool classification MEMBER m_classification NOTIFY classificationChanged);
    Q_PROPERTY(bool autoclassification MEMBER m_autoclassification NOTIFY autoclassificationChanged);
    Q_PROPERTY(int classification_threshold MEMBER m_classification_threshold NOTIFY classificationThresholdChanged);

private:
    int m_threshold_low;
    bool m_classification;
    bool m_autoclassification;
    int m_classification_threshold;

public:
    explicit SegmentationField(QObject *parent = nullptr);

    virtual QImage processImage(const QImage &image);
    virtual ~SegmentationField();

signals:
    void thresholdLowChanged(int);
    void classificationChanged(bool);
    void autoclassificationChanged(bool);
    void classificationThresholdChanged(int);
};



class ThinningFilter : public ImagePreprocess {
    Q_OBJECT;

private:

public:
    explicit ThinningFilter(QObject *parent = nullptr);

    virtual QImage processImage(const QImage &image);
    virtual ~ThinningFilter();

signals:
};

// image processors implementations end

class ImageProcessor : public QObject {
    Q_OBJECT;
private:
    QWidget *prop_group_widget;
    QList<ImagePreprocess*> middleware;

public:
    explicit ImageProcessor(QWidget *prop_group_widget, QObject *parent = nullptr);
    ~ImageProcessor();

    void addMiddleware(ImagePreprocess* mid_elem);
    void clear();

public slots:
    void processImage(const QImage &image);
    void moveUpPreprocess();
    void moveDownPreprocess();
    void deletePreprocess();

signals:
    void resultReady(const QImage &);
    void startCalculating(int, QString); // count of filters
    void currentFilter(int, QString); // number, name
    void finishCalculating();
};

#endif // IMAGEPREPROCESS_H
