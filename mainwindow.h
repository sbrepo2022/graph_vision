#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QActionGroup>
#include <QToolBar>
#include <QProgressDialog>

#include <QThread>

#include "imagepreprocess.h"
#include "exportdialog.h"

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
    void initPreprocessorsMenu();
    void initPresetsMenu();

private:
    Ui::MainWindow *ui;
    QActionGroup *graph_cursor_action_group;
    ImageProcessor *image_processor;

    QPixmap opened_pixmap;
    QImage processed_image;

    ExportDialog *export_dialog;
    QProgressDialog *progress_dialog;

    // threads
    QThread process_image_thread;

public slots:
    void onOpenFile();
    void onExport();
    void onProcessImage();
    void onProcessImageEnd(const QImage &);

    // image process
    void onStartProgressDialog(int count, QString name);
    void onProcessProgressDialog(int number, QString text);
    void onFinishProgressDialog();
    void onProgressDialogCancelled();

    // toolbar
    void arrowCursorMode();
    void addPointCursorMode();

signals:
    void startProcessImage(const QImage &);
};
#endif // MAINWINDOW_H
