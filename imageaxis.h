#ifndef IMAGEAXIS_H
#define IMAGEAXIS_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QCursor>
#include <QPen>
#include <QPainter>

class ImageAxis : public QObject, public QGraphicsItem {
    Q_OBJECT;

public:
    explicit ImageAxis(QObject *parent = nullptr);
    ~ImageAxis();

    void setMode(bool horisontal);
    void setSize(const QPointF &size);
    void setStartPixel(int start_pixel);
    void setPPS(int pps);
    void setStep(double step);

private:
    QPointF size;
    bool horisontal;
    int start_pixel;
    int pps;
    double step;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void axisMoved(int pos);
};

#endif // IMAGEAXIS_H
