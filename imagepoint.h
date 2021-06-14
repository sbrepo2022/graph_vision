#ifndef ImagePoint_H
#define ImagePoint_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QCursor>
#include <QPen>
#include <QPainter>

class ImagePoint : public QObject, public QGraphicsItem {
    Q_OBJECT;

public:
    explicit ImagePoint(QObject *parent = nullptr);
    ~ImagePoint();

    void setSize(const QPointF &size);
    void setPixmap(const QPixmap &pixmap);

private:
    QPixmap pixmap;
    QPointF size;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // ImagePoint_H
