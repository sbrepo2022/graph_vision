#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ExportDialog) {
    ui->setupUi(this);
    scene = nullptr;
    pixmap_item = nullptr;

    connect(ui->buttonSaveImage, SIGNAL(clicked()), this, SLOT(saveImage()));
    connect(ui->spinImageCrop, SIGNAL(valueChanged(int)), this, SLOT(cropWidth(int)));
}

ExportDialog::~ExportDialog() {
    delete ui;
}

void ExportDialog::setExportImage(const QImage &export_image) {
    this->export_image = export_image;
    if (! export_image.isNull()) {
        ui->spinImageCrop->setMaximum(export_image.width() < export_image.height() ? export_image.width() / 2 : export_image.height() / 2);
    }
    else {
        ui->spinImageCrop->setMaximum(0);
    }
    if (! export_image.isNull()) {
        if (scene != nullptr)
            delete scene;
        scene = new QGraphicsScene();
        ui->previewImage->setScene(scene);
        pixmap_item = ui->previewImage->scene()->addPixmap(QPixmap::fromImage(export_image));

        updatePreview();
    }
}

void ExportDialog::updatePreview() {
    int crop_width = ui->spinImageCrop->value();
    if (! export_image.isNull())
        transformed_image = export_image.copy(crop_width, crop_width, export_image.width() - crop_width * 2, export_image.height() - crop_width * 2);
    if (pixmap_item != nullptr)
        pixmap_item->setPixmap(QPixmap::fromImage(transformed_image));
}

void ExportDialog::saveImage() {
    if (! export_image.isNull()) {
        updatePreview();
        QString filename = QFileDialog::getSaveFileName(this, "Save Image", "/", "Image Files(*.png *.jpg *.jpeg *.bmp)");
        if (filename != "") {
            if (transformed_image.save(filename)) {
                accept();
            }
            else {
                QMessageBox::warning(this, "Save Image", "Export image failed");
                reject();
            }
        }
    }
    else {
        QMessageBox::warning(this, "Save Image", "Image not found (generate it previously)");
    }
}

void ExportDialog::cropWidth(int crop_width) {
    this->crop_width = crop_width;
    updatePreview();
}
