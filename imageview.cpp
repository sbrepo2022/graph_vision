#include "imageview.h"

ImageView::ImageView(QWidget *parent) : QGraphicsView(parent) {
    this->bg_image = nullptr;
    this->x_axis = nullptr;
    this->y_axis = nullptr;
    this->scale_factor = 1;

    this->scene = new QGraphicsScene();
    this->setScene(this->scene);

    // styles
    this->setBackgroundBrush(QBrush(QColor(240, 240, 240)));
}

ImageView::~ImageView() {

}

void ImageView::initSceneItems(const QPixmap &pixmap) {
    this->deleteSceneItems();

    this->bg_image = this->scene->addPixmap(pixmap);
    this->scene->setSceneRect(pixmap.rect());

    QRectF scene_rect = this->scene->sceneRect();

    this->x_axis = new ImageAxis(QPointF(scene_rect.width(), 10));
    this->x_axis->setPos(0, scene_rect.height() / 2 - 5);
    this->x_axis->setMode(true);
    this->x_axis->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    this->scene->addItem(x_axis);

    this->y_axis = new ImageAxis(QPointF(10, scene_rect.height()));
    this->y_axis->setPos(scene_rect.width() / 2 - 5, 0);
    this->y_axis->setMode(false);
    this->y_axis->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    this->scene->addItem(y_axis);

    this->resizeContent();
}

void ImageView::deleteSceneItems() {
    this->scene->clear();

    this->bg_image = nullptr;
    this->x_axis = nullptr;
    this->y_axis = nullptr;
}

void ImageView::resizeContent() {
    QRectF scene_rect = this->scene->sceneRect();
    scene_rect.setWidth(scene_rect.width() * (1.0f / SCALE_FACTOR(this->scale_factor)));
    scene_rect.setHeight(scene_rect.height() * (1.0f / SCALE_FACTOR(this->scale_factor)));

    this->fitInView(scene_rect, Qt::KeepAspectRatio);

    qDebug() << this->rect();
    qDebug() << this->sceneRect();
    /*QTransform transform = this->transform();
    QPointF m_size;
    if (this->x_axis != nullptr)
        this->x_axis->setSize(QPointF(m_size.x(), 10));
    if (this->y_axis != nullptr)
        this->y_axis->setSize(QPointF(10, m_size.y()));*/
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
