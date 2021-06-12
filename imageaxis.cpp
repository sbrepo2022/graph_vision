#include "imageaxis.h"

ImageAxis::ImageAxis(QObject *parent) : QObject(parent), QGraphicsItem() {

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
    QPen penRedDots(QColor(240, 50, 50), 3, Qt::DashDotLine, Qt::RoundCap);
    QPen penGreenDots(QColor(50, 240, 50), 3, Qt::DashDotLine, Qt::RoundCap);
    if (this->horisontal) {
        painter->setPen(penRedDots);
        painter->drawLine(0, size.y() / 2, size.x(), size.y() / 2);
    }
    else {
        painter->setPen(penGreenDots);
        painter->drawLine(size.x() / 2, 0, size.x() / 2, size.y());
    }

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void ImageAxis::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QPointF scene_pos;
    if (this->horisontal) {
        scene_pos = mapToScene(QPoint(0, event->pos().y()));
        this->setPos(scene_pos);
        emit this->axisMoved(scene_pos.y() + size.y() / 2);
    }
    else {
        scene_pos = mapToScene(QPoint(event->pos().x(), 0));
        this->setPos(scene_pos);
        emit this->axisMoved(scene_pos.x() + size.x() / 2);
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
