#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // initialize ui
    this->initUi();

    // window settings
    this->showMaximized();

    // init dialogs
    export_dialog = new ExportDialog(this);

    // initialize toolbar
    graph_cursor_action_group = new QActionGroup(this);
    graph_cursor_action_group->setExclusive(true);
    graph_cursor_action_group->addAction(ui->actionCursor);
    graph_cursor_action_group->addAction(ui->actionAddPoint);
    ui->actionCursor->setChecked(true);
    ui->toolBar->addActions(graph_cursor_action_group->actions());

    // connect signal-slots
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    QObject::connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(onExport()));
    QObject::connect(ui->actionRunPreprocess, SIGNAL(triggered()), this, SLOT(onProcessImage()));

    QObject::connect(ui->actionCursor, SIGNAL(triggered()), this, SLOT(arrowCursorMode()));
    QObject::connect(ui->actionAddPoint, SIGNAL(triggered()), this, SLOT(addPointCursorMode()));

    QObject::connect(ui->sliderImageScale, SIGNAL(valueChanged(int)), ui->graphicsViewImage, SLOT(setScaleFactor(int)));
    QObject::connect(ui->checkXAxis, SIGNAL(stateChanged(int)), ui->graphicsViewImage, SLOT(xAxisVisible(int)));
    QObject::connect(ui->checkYAxis, SIGNAL(stateChanged(int)), ui->graphicsViewImage, SLOT(yAxisVisible(int)));
    QObject::connect(ui->checkImagePoints, SIGNAL(stateChanged(int)), ui->graphicsViewImage, SLOT(imagePointsVisible(int)));
    QObject::connect(ui->checkProcessedImage, SIGNAL(stateChanged(int)), ui->graphicsViewImage, SLOT(showProcessedImage(int)));
    QObject::connect(ui->spinPPSX, SIGNAL(valueChanged(int)), ui->graphicsViewImage, SLOT(setPPSX(int)));
    QObject::connect(ui->spinPPSY, SIGNAL(valueChanged(int)), ui->graphicsViewImage, SLOT(setPPSY(int)));
    QObject::connect(ui->doubleSpinStepX, SIGNAL(valueChanged(double)), ui->graphicsViewImage, SLOT(setStepX(double)));
    QObject::connect(ui->doubleSpinStepY, SIGNAL(valueChanged(double)), ui->graphicsViewImage, SLOT(setStepY(double)));

    // init image processor
    image_processor = new ImageProcessor(ui->groupImageProcess);
    image_processor->addMiddleware(new GaussianBlur());
    image_processor->addMiddleware(new CannyFilter());

    image_processor->moveToThread(&process_image_thread);
    connect(image_processor, &ImageProcessor::resultReady, this, &MainWindow::onProcessImageEnd);
    connect(this, &MainWindow::startProcessImage, image_processor, &ImageProcessor::processImage);
    connect(image_processor, &ImageProcessor::startCalculating, this, &MainWindow::onStartProgressDialog);
    connect(image_processor, &ImageProcessor::currentFilter, this, &MainWindow::onProcessProgressDialog);
    connect(image_processor, &ImageProcessor::finishCalculating, this, &MainWindow::onFinishProgressDialog);
    process_image_thread.start();

    initPreprocessorsMenu();
    initPresetsMenu();
}

MainWindow::~MainWindow() {
    process_image_thread.quit();
    process_image_thread.wait();

    delete ui;
    delete image_processor;
}

void MainWindow::initUi() {
    ui->splitterImage->setStretchFactor(0, 1);
    ui->splitterImage->setStretchFactor(1, 0);
    ui->splitterGraph->setStretchFactor(0, 1);
    ui->splitterGraph->setStretchFactor(1, 0);
    ui->splitterMain->setSizes(QList<int>({INT_MAX, INT_MAX}));
}

void MainWindow::initPreprocessorsMenu() {
    QAction *monochrome_gradient = new QAction("Monochrome gradient");
    connect(monochrome_gradient, &QAction::triggered, this, [=]() {
        image_processor->addMiddleware(new MonochromeGradientImage());
    });
    ui->menuAddFilter->addAction(monochrome_gradient);

    QAction *gaussian_blur = new QAction("Gaussian blur");
    connect(gaussian_blur, &QAction::triggered, this, [=]() {
        image_processor->addMiddleware(new GaussianBlur());
    });
    ui->menuAddFilter->addAction(gaussian_blur);

    QAction *canny_filter = new QAction("Canny filter");
    connect(canny_filter, &QAction::triggered, this, [=]() {
        image_processor->addMiddleware(new CannyFilter());
    });
    ui->menuAddFilter->addAction(canny_filter);

    QAction *color_gradient_filter = new QAction("Color gradient filter");
    connect(color_gradient_filter, &QAction::triggered, this, [=]() {
        image_processor->addMiddleware(new ColorGradientField());
    });
    ui->menuAddFilter->addAction(color_gradient_filter);
}

void MainWindow::initPresetsMenu() {
    QAction *edge_analisys = new QAction("Edge analysis");
    connect(edge_analisys, &QAction::triggered, this, [=]() {
        image_processor->clear();
        image_processor->addMiddleware(new GaussianBlur());
        image_processor->addMiddleware(new CannyFilter());
    });
    ui->menuPresets->addAction(edge_analisys);

    QAction *gradient_analisys = new QAction("Gradient analysis");
    connect(gradient_analisys, &QAction::triggered, this, [=]() {
        image_processor->clear();
        image_processor->addMiddleware(new GaussianBlur());
        image_processor->addMiddleware(new ColorGradientField());
    });
    ui->menuPresets->addAction(gradient_analisys);
}

// slots
void MainWindow::onOpenFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Image", "/", "Image Files (*.png *.jpg *.bmp)");
    if (filename != "") {
        opened_pixmap = QPixmap(filename);
        if (opened_pixmap.isNull()) {
            QMessageBox::warning(this, "Loading pixmap", "Cannot load image file");
        }
        else {
            ui->graphicsViewImage->initSceneItems(opened_pixmap);
            ui->graphicsViewImage->xAxisVisible(ui->checkXAxis->isChecked());
            ui->graphicsViewImage->yAxisVisible(ui->checkYAxis->isChecked());
            ui->graphicsViewImage->setPPSX(ui->spinPPSX->value());
            ui->graphicsViewImage->setPPSY(ui->spinPPSY->value());
            ui->graphicsViewImage->setStepX(ui->doubleSpinStepX->value());
            ui->graphicsViewImage->setStepY(ui->doubleSpinStepY->value());

            ui->checkProcessedImage->setDisabled(true);
            ui->checkProcessedImage->setChecked(false);
        }
    }
}

void MainWindow::onExport() {
    export_dialog->setExportImage(processed_image);
    export_dialog->exec();
}

void MainWindow::onProcessImage() {
    if (! opened_pixmap.isNull()) {
        emit startProcessImage(opened_pixmap.toImage());
    }
}

void MainWindow::onProcessImageEnd(const QImage &result) {
    processed_image = result;
    ui->graphicsViewImage->setProcessedImage(QPixmap::fromImage(processed_image));
    ui->checkProcessedImage->setDisabled(false);
}

void MainWindow::onStartProgressDialog(int count, QString name) {
    progress_dialog = new QProgressDialog(name, "cancel", 0, count);
    connect(progress_dialog, &QProgressDialog::canceled, this, &MainWindow::onProgressDialogCancelled);
    progress_dialog->open();
}

void MainWindow::onProcessProgressDialog(int number, QString text) {
    progress_dialog->setValue(number);
    progress_dialog->setLabelText(text);
}

void MainWindow::onFinishProgressDialog() {
    progress_dialog->setValue(progress_dialog->maximum());
    progress_dialog->deleteLater();
}

void MainWindow::onProgressDialogCancelled() {

}

void MainWindow::arrowCursorMode() {
    ui->graphicsViewImage->setCursorMode(ImageViewCursorMode::Arrow);
}

void MainWindow::addPointCursorMode() {
    ui->graphicsViewImage->setCursorMode(ImageViewCursorMode::AddPoint);
}
