#include "imageview.h"

ImageView::ImageView(QWidget *parent) : QGraphicsView(parent) {
    this->bg_image = nullptr;
    this->x_axis = nullptr;
    this->y_axis = nullptr;

    this->scale_factor = 1;
    this->pps_x = this->pps_y = 50;

    this->scene = new QGraphicsScene();
    this->setScene(this->scene);

    // styles
    this->setBackgroundBrush(QBrush(QColor(240, 240, 240)));
}

ImageView::~ImageView() {
    this->deleteSceneItems();
}

void ImageView::initSceneItems(const QPixmap &pixmap) {
    this->deleteSceneItems();

    this->bg_image = this->scene->addPixmap(pixmap);
    this->scene->setSceneRect(pixmap.rect());

    QRectF scene_rect = this->scene->sceneRect();

    this->x_axis = new ImageAxis();
    this->x_axis->setPos(0, scene_rect.height() / 2 - 5);
    this->x_axis->setMode(true);
    this->x_axis->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    this->scene->addItem(x_axis);
    this->start_pixel_x = scene_rect.width() / 2;

    this->y_axis = new ImageAxis();
    this->y_axis->setPos(scene_rect.width() / 2 - 5, 0);
    this->y_axis->setMode(false);
    this->y_axis->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    this->scene->addItem(y_axis);
    this->start_pixel_y = scene_rect.height() / 2;

    QObject::connect(this->x_axis, &ImageAxis::axisMoved, this, &ImageView::setStartPixelY);
    QObject::connect(this->y_axis, &ImageAxis::axisMoved, this, &ImageView::setStartPixelX);

    this->resizeContent();
}

void ImageView::deleteSceneItems() {
    if (this->x_axis != nullptr)
        QObject::disconnect(this->x_axis, &ImageAxis::axisMoved, this, &ImageView::setStartPixelY);
    if (this->y_axis != nullptr)
        QObject::disconnect(this->y_axis, &ImageAxis::axisMoved, this, &ImageView::setStartPixelX);

    this->scene->clear();

    this->bg_image = nullptr;
    this->x_axis = nullptr;
    this->y_axis = nullptr;
}

void ImageView::resizeContent() {
    QRectF scene_rect = this->scene->sceneRect();

    this->fitInView(scene_rect, Qt::KeepAspectRatio);
    scale(SCALE_FACTOR(this->scale_factor), SCALE_FACTOR(this->scale_factor));

    QPoint m_start = mapFromScene(QPointF(0, 0));
    QPoint m_end = mapFromScene(QPointF(scene_rect.width(), scene_rect.height()));
    QPoint m_size = m_end - m_start;
    if (this->x_axis != nullptr)
        this->x_axis->setSize(QPointF(m_size.x(), 10));
    if (this->y_axis != nullptr)
        this->y_axis->setSize(QPointF(10, m_size.y()));
}

void ImageView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);

    this->resizeContent();
}

// slots

void ImageView::setScaleFactor(int scale_factor) {
    this->scale_factor = scale_factor;
    this->resizeContent();
}

void ImageView::xAxisVisible(int state) {
    if (this->x_axis != nullptr) {
        this->x_axis->setVisible(state);
    }
}

void ImageView::yAxisVisible(int state) {
    if (this->y_axis != nullptr) {
        this->y_axis->setVisible(state);
    }
}

void ImageView::setStartPixelX(int start_pixel) {
    this->start_pixel_x = start_pixel;
}

void ImageView::setStartPixelY(int start_pixel) {
    this->start_pixel_y = start_pixel;
}

void ImageView::setPPSX(int pps) {
    this->pps_x = pps;
}

void ImageView::setPPSY(int pps){
    this->pps_y = pps;
}

void ImageView::setStepX(double step) {
    this->step_x = step;
}

void ImageView::setStepY(double step) {
    this->step_y = step;
}
