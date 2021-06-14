#include "imagepoint.h"

ImagePoint::ImagePoint(QObject *parent) : QObject(parent), QGraphicsItem() {

}

ImagePoint::~ImagePoint() {

}

void ImagePoint::setSize(const QPointF &size) {
    this->size = size;
}

void ImagePoint::setPixmap(const QPixmap &pixmap) {
    this->pixmap = pixmap;
}

QRectF ImagePoint::boundingRect() const {
    return QRectF(-(size.x() / 2), -(size.y() / 2), size.x(), size.y());
}

void ImagePoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->drawPixmap(QRectF(-(size.x() / 2), -(size.y() / 2), size.x(), size.y()), pixmap, pixmap.rect());

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void ImagePoint::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QPointF scene_pos = mapToScene(QPoint(event->pos().x(), event->pos().y()));
    this->setPos(scene_pos);
}

void ImagePoint::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    this->setCursor(QCursor(Qt::ClosedHandCursor));
    Q_UNUSED(event);
}

void ImagePoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    this->setCursor(QCursor(Qt::ArrowCursor));
    Q_UNUSED(event);
}
