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
#include "imagepoint.h"

#define SCALE_FACTOR(scale_factor) (1 + (scale_factor - 1) * 0.5)

enum ImageViewCursorMode {Arrow, AddPoint};

class ImageView : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = 0);
    ~ImageView();

    void initSceneItems(const QPixmap &pixmap);
    void setProcessedImage(const QPixmap &processed_pixmap);

    void deleteSceneItems();
    void resizeContent();

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *bg_image;
    ImageAxis *x_axis, *y_axis;
    QVector<ImagePoint*> points;
    QPixmap opened_pixmap, processed_pixmap;

    // custom cursors
    QCursor add_point_cursor;

    int scale_factor;
    int start_pixel_x, start_pixel_y;
    int pps_x, pps_y;
    int step_x, step_y;
    bool show_processed_image_state;
    ImageViewCursorMode cursor_mode;

    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);

public slots:
    void setCursorMode(ImageViewCursorMode mode);

    void setScaleFactor(int scale_factor);
    void xAxisVisible(int state);
    void yAxisVisible(int state);
    void showProcessedImage(int state);
    void setStartPixelX(int start_pixel);
    void setStartPixelY(int start_pixel);
    void setPPSX(int pps);
    void setPPSY(int pps);
    void setStepX(double step);
    void setStepY(double step);
};

#endif // IMAGEVIEW_H
