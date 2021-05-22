#ifndef IMAGEAXIS_H
#define IMAGEAXIS_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QCursor>
#include <QPen>
#include <QPainter>

class ImageAxis : public QGraphicsItem {

public:
    ImageAxis(const QPointF &size);
    ~ImageAxis();

    void setMode(bool horisontal);
    void setSize(const QPointF &size);

private:
    QPointF size;
    bool horisontal;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // IMAGEAXIS_H
