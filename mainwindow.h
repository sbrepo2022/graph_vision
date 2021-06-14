#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QActionGroup>
#include <QToolBar>
#include "imagepreprocess.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void initUi();

private:
    Ui::MainWindow *ui;
    QActionGroup *graph_cursor_action_group;
    MonochromeImage *monochrome_image;
    ImageProcessor image_processor;

    QPixmap opened_pixmap;
    QImage processed_image;

public slots:
    void onOpenFile();
    void onProcessImage();

    void setThreshold(int threshold);

    // toolbar
    void arrowCursorMode();
    void addPointCursorMode();
};
#endif // MAINWINDOW_H
