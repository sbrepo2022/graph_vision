#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // initialize ui
    this->initUi();

    // initialize toolbar
    graph_cursor_action_group = new QActionGroup(this);
    graph_cursor_action_group->setExclusive(true);
    graph_cursor_action_group->addAction(ui->actionCursor);
    graph_cursor_action_group->addAction(ui->actionAddPoint);
    ui->actionCursor->setChecked(true);
    ui->toolBar->addActions(graph_cursor_action_group->actions());

    // connect signal-slots
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    QObject::connect(ui->actionRunPreprocess, SIGNAL(triggered()), this, SLOT(onProcessImage()));

    QObject::connect(ui->actionCursor, SIGNAL(triggered()), this, SLOT(arrowCursorMode()));
    QObject::connect(ui->actionAddPoint, SIGNAL(triggered()), this, SLOT(addPointCursorMode()));

    QObject::connect(ui->sliderImageScale, SIGNAL(valueChanged(int)), ui->graphicsViewImage, SLOT(setScaleFactor(int)));
    QObject::connect(ui->checkXAxis, SIGNAL(stateChanged(int)), ui->graphicsViewImage, SLOT(xAxisVisible(int)));
    QObject::connect(ui->checkYAxis, SIGNAL(stateChanged(int)), ui->graphicsViewImage, SLOT(yAxisVisible(int)));
    QObject::connect(ui->checkProcessedImage, SIGNAL(stateChanged(int)), ui->graphicsViewImage, SLOT(showProcessedImage(int)));
    QObject::connect(ui->spinPPSX, SIGNAL(valueChanged(int)), ui->graphicsViewImage, SLOT(setPPSX(int)));
    QObject::connect(ui->spinPPSY, SIGNAL(valueChanged(int)), ui->graphicsViewImage, SLOT(setPPSY(int)));
    QObject::connect(ui->doubleSpinStepX, SIGNAL(valueChanged(double)), ui->graphicsViewImage, SLOT(setStepX(double)));
    QObject::connect(ui->doubleSpinStepY, SIGNAL(valueChanged(double)), ui->graphicsViewImage, SLOT(setStepY(double)));
    QObject::connect(ui->spinThreshold, SIGNAL(valueChanged(int)), this, SLOT(setThreshold(int)));

    // init image processor
    monochrome_image = new MonochromeImage();
    monochrome_image->setThreshold(ui->spinThreshold->value());
    image_processor.addMiddleware(monochrome_image);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initUi() {
    ui->splitterImage->setStretchFactor(0, 1);
    ui->splitterImage->setStretchFactor(1, 0);
    ui->splitterGraph->setStretchFactor(0, 1);
    ui->splitterGraph->setStretchFactor(1, 0);
    ui->splitterMain->setSizes(QList<int>({INT_MAX, INT_MAX}));
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

void MainWindow::onProcessImage() {
    if (! opened_pixmap.isNull()) {
        processed_image = image_processor.processImage(opened_pixmap.toImage());
        ui->graphicsViewImage->setProcessedImage(QPixmap::fromImage(processed_image));
        ui->checkProcessedImage->setDisabled(false);
    }
}

void MainWindow::setThreshold(int threshold) {
    monochrome_image->setThreshold(threshold);
}

void MainWindow::arrowCursorMode() {
    ui->graphicsViewImage->setCursorMode(ImageViewCursorMode::Arrow);
}

void MainWindow::addPointCursorMode() {
    ui->graphicsViewImage->setCursorMode(ImageViewCursorMode::AddPoint);
}
