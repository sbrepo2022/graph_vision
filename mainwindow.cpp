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
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initUi() {
    ui->splitterImage->setStretchFactor(0, 1);
    ui->splitterImage->setStretchFactor(1, 0);
    ui->splitterGraph->setStretchFactor(0, 1);
    ui->splitterGraph->setStretchFactor(1, 0);
}

// slots
void MainWindow::onOpenFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Image", "/", "Image Files (*.png *.jpg *.bmp)");
    if (filename != "") {
        QPixmap pixmap(filename);
        ui->graphicsViewImage->initSceneItems(pixmap);
        if (pixmap.isNull()) {
            QMessageBox::warning(this, "Loading pixmap", "Cannot load image file");
        }
    }

}

