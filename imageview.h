#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QApplication>
#include <QResizeEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QObject>
#include <QDebug>

#include "imageaxis.h"

#define SCALE_FACTOR(scale_factor) (1 + (scale_factor - 1) * 0.5)

class ImageView : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = 0);
    ~ImageView();

    void initSceneItems(const QPixmap &pixmap);
    void deleteSceneItems();
    void resizeContent();

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *bg_image;
    ImageAxis *x_axis, *y_axis;

    int scale_factor;
    int start_pixel_x, start_pixel_y;
    int pps_x, pps_y;
    int step_x, step_y;

    void resizeEvent(QResizeEvent *event);

public slots:
    void setScaleFactor(int scale_factor);
    void xAxisVisible(int state);
    void yAxisVisible(int state);
    void setStartPixelX(int start_pixel);
    void setStartPixelY(int start_pixel);
    void setPPSX(int pps);
    void setPPSY(int pps);
    void setStepX(double step);
    void setStepY(double step);
};

#endif // IMAGEVIEW_H
