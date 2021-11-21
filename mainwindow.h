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
#include "graphpreprocess.h"
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
    GraphProcessor *graph_processor;
    GraphProcessorGraph *graph_processor_graph;

    QPixmap opened_pixmap;
    QImage processed_image;

    ExportDialog *export_dialog;
    QProgressDialog *progress_dialog;

    // threads
    QThread process_image_thread;
    QThread process_graph_thread;

public slots:
    void onOpenFile();
    void onExport();
    void graphModeChanged(int);
    void onProcessImage();
    void onProcessImageEnd(const QImage &);
    void onProcessGraphEnd(VectorizationProduct); // PRINT GRAPH HERE
    void onProcessGraphEnd2(VectorizationProductGraph); // PRINT GRAPH HERE
    void onProcessGraph();
    void onProcessAll();
    void onProcessAllEnd();

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
    void startProcessGraph(const QImage &);
    void startProcessGraph2(const QImage &);
    void endProcessImage();
    void endProcessGraph();
};
#endif // MAINWINDOW_H
