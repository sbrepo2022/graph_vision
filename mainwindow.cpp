#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // initialize ui
    this->initUi();

    // connect signal-slots
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    QObject::connect(ui->sliderImageScale, SIGNAL(valueChanged(int)), ui->graphicsViewImage, SLOT(setScaleFactor(int)));
    QObject::connect(ui->checkXAxis, SIGNAL(stateChanged(int)), ui->graphicsViewImage, SLOT(xAxisVisible(int)));
    QObject::connect(ui->checkYAxis, SIGNAL(stateChanged(int)), ui->graphicsViewImage, SLOT(yAxisVisible(int)));
    QObject::connect(ui->spinPPSX, SIGNAL(valueChanged(int)), ui->graphicsViewImage, SLOT(setPPSX(int)));
    QObject::connect(ui->spinPPSY, SIGNAL(valueChanged(int)), ui->graphicsViewImage, SLOT(setPPSY(int)));
    QObject::connect(ui->doubleSpinStepX, SIGNAL(valueChanged(double)), ui->graphicsViewImage, SLOT(setStepX(double)));
    QObject::connect(ui->doubleSpinStepY, SIGNAL(valueChanged(double)), ui->graphicsViewImage, SLOT(setStepY(double)));
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
        QPixmap pixmap(filename);
        ui->graphicsViewImage->initSceneItems(pixmap);
        ui->graphicsViewImage->setPPSX(ui->spinPPSX->value());
        ui->graphicsViewImage->setPPSY(ui->spinPPSY->value());
        ui->graphicsViewImage->setStepX(ui->doubleSpinStepX->value());
        ui->graphicsViewImage->setStepY(ui->doubleSpinStepY->value());
        if (pixmap.isNull()) {
            QMessageBox::warning(this, "Loading pixmap", "Cannot load image file");
        }
    }

}

