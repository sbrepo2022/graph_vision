#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QPalette>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

namespace Ui {
    class ExportDialog;
}

class ExportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = nullptr);
    ~ExportDialog();

    void setExportImage(const QImage &export_image);

private:
    Ui::ExportDialog *ui;
    QImage export_image, transformed_image;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmap_item;

    int crop_width;

protected:
    void updatePreview();

public slots:
    void saveImage();

    void cropWidth(int crop_width);
};

#endif // EXPORTDIALOG_H
