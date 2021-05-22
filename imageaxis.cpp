#include "imageaxis.h"

ImageAxis::ImageAxis(const QPointF &size) : QGraphicsItem() {
    this->size = size;
}

ImageAxis::~ImageAxis() {

}

void ImageAxis::setMode(bool horisontal) {
    this->horisontal = horisontal;
}

void ImageAxis::setSize(const QPointF &size) {
    this->size = size;
}

QRectF ImageAxis::boundingRect() const {
    return QRectF(0, 0, size.x(), size.y());
}

void ImageAxis::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen penRed(Qt::red, 4, Qt::DashDotLine, Qt::RoundCap);
    QPen penGreen(Qt::green, 4, Qt::DashDotLine, Qt::RoundCap);
    if (this->horisontal) {
        painter->setPen(penRed);
        painter->drawLine(0, size.y() / 2, size.x(), size.y() / 2);
    }
    else {
        painter->setPen(penGreen);
        painter->drawLine(size.x() / 2, 0, size.x() / 2, size.y());
    }

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void ImageAxis::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (this->horisontal) {
        this->setPos(mapToScene(QPointF(this->pos().x(), event->pos().y())));
    }
    else {
        this->setPos(mapToScene(QPointF(event->pos().x(), this->pos().y())));
    }
}

void ImageAxis::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    this->setCursor(QCursor(Qt::ClosedHandCursor));
    Q_UNUSED(event);
}

void ImageAxis::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    this->setCursor(QCursor(Qt::ArrowCursor));
    Q_UNUSED(event);
}
