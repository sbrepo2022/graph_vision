#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

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


    void resizeEvent(QResizeEvent *event);

public slots:
    void setScaleFactor(int scale_factor);
    void xAxisVisible(int state);
    void yAxisVisible(int state);
};

#endif // IMAGEVIEW_H
