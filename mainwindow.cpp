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

    // initialize chart view
    ui->graphicsViewGraph->setRubberBand(QChartView::RectangleRubberBand);

    // connect signal-slots
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    QObject::connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(onExport()));
    QObject::connect(ui->actionRunPreprocess, SIGNAL(triggered()), this, SLOT(onProcessImage()));
    QObject::connect(ui->actionRunGraph, SIGNAL(triggered()), this, SLOT(onProcessGraph()));
    QObject::connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(onProcessAll()));

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

    QObject::connect(ui->comboBoxGraphMode, SIGNAL(currentIndexChanged(int)), this, SLOT(graphModeChanged(int)));

    // init image processor
    image_processor = new ImageProcessor(ui->groupImageProcess);
    image_processor->addMiddleware(new GaussianBlur());
    image_processor->addMiddleware(new ColorGradientField());
    image_processor->addMiddleware(new SegmentationField());
    image_processor->addMiddleware(new ThinningFilter());

    image_processor->moveToThread(&process_image_thread);
    connect(image_processor, &ImageProcessor::resultReady, this, &MainWindow::onProcessImageEnd);
    connect(this, &MainWindow::startProcessImage, image_processor, &ImageProcessor::processImage);
    connect(image_processor, &ImageProcessor::startCalculating, this, &MainWindow::onStartProgressDialog);
    connect(image_processor, &ImageProcessor::currentFilter, this, &MainWindow::onProcessProgressDialog);
    connect(image_processor, &ImageProcessor::finishCalculating, this, &MainWindow::onFinishProgressDialog);
    process_image_thread.start();

    initPreprocessorsMenu();
    initPresetsMenu();

    // init graph processor
    QList<VectorTransforms*> vector_trans_filters = {new VectorNoiseClearing(), new VectorMerge(), new VectorNoiseClearing()};

    graph_processor = new GraphProcessor(ui->groupGraphProcess);
    graph_processor->setMiddleware(new LinearVectorization(), vector_trans_filters);

    graph_processor->moveToThread(&process_graph_thread);
    connect(graph_processor, &GraphProcessor::resultReady, this, &MainWindow::onProcessGraphEnd);
    connect(this, &MainWindow::startProcessGraph, graph_processor, &GraphProcessor::processGraph);
    connect(graph_processor, &GraphProcessor::startCalculating, this, &MainWindow::onStartProgressDialog);
    connect(graph_processor, &GraphProcessor::currentFilter, this, &MainWindow::onProcessProgressDialog);
    connect(graph_processor, &GraphProcessor::finishCalculating, this, &MainWindow::onFinishProgressDialog);

    // init graph processor graph
    graph_processor_graph = new GraphProcessorGraph(ui->groupGraphProcessGraph);
    graph_processor_graph->setMiddleware(new LinearVectorizationGraph());

    graph_processor_graph->moveToThread(&process_graph_thread);
    connect(graph_processor_graph, &GraphProcessorGraph::resultReady, this, &MainWindow::onProcessGraphEnd2);
    connect(this, &MainWindow::startProcessGraph2, graph_processor_graph, &GraphProcessorGraph::processGraph);
    connect(graph_processor_graph, &GraphProcessorGraph::startCalculating, this, &MainWindow::onStartProgressDialog);
    connect(graph_processor_graph, &GraphProcessorGraph::currentFilter, this, &MainWindow::onProcessProgressDialog);
    connect(graph_processor_graph, &GraphProcessorGraph::finishCalculating, this, &MainWindow::onFinishProgressDialog);
    process_graph_thread.start();
}

MainWindow::~MainWindow() {
    process_image_thread.quit();
    process_image_thread.wait();

    process_graph_thread.quit();
    process_graph_thread.wait();

    delete ui;
    delete image_processor;
    delete graph_processor;
}

void MainWindow::initUi() {
    ui->groupGraphProcessGraph->setVisible(false);
    ui->splitterImage->setStretchFactor(0, 1);
    ui->splitterImage->setStretchFactor(1, 0);
    ui->splitterGraph->setStretchFactor(0, 1);
    ui->splitterGraph->setStretchFactor(1, 0);
    ui->splitterImageProcess->setStretchFactor(0, 0);
    ui->splitterImageProcess->setStretchFactor(1, 1);
    ui->splitterGraphProcess->setStretchFactor(0, 0);
    ui->splitterGraphProcess->setStretchFactor(1, 1);
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

    QAction *segmentation_filter = new QAction("Segmentation filter");
    connect(segmentation_filter, &QAction::triggered, this, [=]() {
        image_processor->addMiddleware(new SegmentationField());
    });
    ui->menuAddFilter->addAction(segmentation_filter);

    QAction *thinning_filter = new QAction("Thinning filter");
    connect(thinning_filter, &QAction::triggered, this, [=]() {
        image_processor->addMiddleware(new ThinningFilter());
    });
    ui->menuAddFilter->addAction(thinning_filter);
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

    QAction *graphic_analisys = new QAction("Graphic analysis");
    connect(graphic_analisys, &QAction::triggered, this, [=]() {
        image_processor->clear();
        image_processor->addMiddleware(new GaussianBlur());
        image_processor->addMiddleware(new ColorGradientField());
        image_processor->addMiddleware(new SegmentationField());
        image_processor->addMiddleware(new ThinningFilter());
    });
    ui->menuPresets->addAction(graphic_analisys);
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

void MainWindow::graphModeChanged(int index) {
    QObjectList children = ui->widgetGraphModes->children();
    foreach(QObject *child, children) {
        QWidget *child_widget = qobject_cast<QWidget*> (child);
        if (child_widget) {
            child_widget->setVisible(false);
        }
    }
    if (index == 0) {
        ui->groupGraphProcess->setVisible(true);
    }
    else if (index == 1) {
        ui->groupGraphProcessGraph->setVisible(true);
    }
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
    emit endProcessImage();
}

void MainWindow::onProcessGraphEnd(VectorizationProduct result) {
    int start_x = ui->graphicsViewImage->getStartPixelX(), start_y = processed_image.height() - ui->graphicsViewImage->getStartPixelY();
    int pps_x = ui->graphicsViewImage->getPPSX(), pps_y = ui->graphicsViewImage->getPPSY();
    double step_x = ui->graphicsViewImage->getStepX(), step_y = ui->graphicsViewImage->getStepY();

    double g_start_x = (double)(0 - start_x) * (step_x / pps_x), g_start_y = (double)(0 - start_y) * (step_y / pps_y);
    double g_end_x = (double)(processed_image.width() - start_x) * (step_x / pps_x), g_end_y = (double)(processed_image.height() - start_y) * (step_y / pps_y);

    QChart *chart = new QChart();

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("x");
    axisX->setLabelFormat("%g");
    axisX->setTickInterval(100.0);
    axisX->setTickAnchor(0.0);
    axisX->setTickType(QValueAxis::TicksDynamic);
    axisX->setRange(g_start_x, g_end_x);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("y");
    axisY->setLabelFormat("%g");
    axisY->setTickInterval(100.0);
    axisY->setTickAnchor(0.0);
    axisY->setTickType(QValueAxis::TicksDynamic);
    axisY->setRange(g_start_y, g_end_y);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for (auto it = result.begin(); it != result.end(); it++) {
        QLineSeries *series = new QLineSeries();
        for (auto it_2 = it->begin(); it_2 != it->end(); it_2++) {
            QPointF point = *it_2;
            point.setY(processed_image.height() - point.y());
            point.setX( (double)(point.x() - start_x) * (step_x / pps_x) );
            point.setY( (double)(point.y() - start_y) * (step_y / pps_y) );
            *series << point;
        }
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    this->ui->graphicsViewGraph->setChart(chart);
    emit endProcessGraph();
}

void MainWindow::onProcessGraphEnd2(VectorizationProductGraph result) {
    int start_x = ui->graphicsViewImage->getStartPixelX(), start_y = processed_image.height() - ui->graphicsViewImage->getStartPixelY();
    int pps_x = ui->graphicsViewImage->getPPSX(), pps_y = ui->graphicsViewImage->getPPSY();
    double step_x = ui->graphicsViewImage->getStepX(), step_y = ui->graphicsViewImage->getStepY();

    double g_start_x = (double)(0 - start_x) * (step_x / pps_x), g_start_y = (double)(0 - start_y) * (step_y / pps_y);
    double g_end_x = (double)(processed_image.width() - start_x) * (step_x / pps_x), g_end_y = (double)(processed_image.height() - start_y) * (step_y / pps_y);

    QChart *chart = new QChart();

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("x");
    axisX->setLabelFormat("%g");
    axisX->setTickInterval(100.0);
    axisX->setTickAnchor(0.0);
    axisX->setTickType(QValueAxis::TicksDynamic);
    axisX->setRange(g_start_x, g_end_x);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("y");
    axisY->setLabelFormat("%g");
    axisY->setTickInterval(100.0);
    axisY->setTickAnchor(0.0);
    axisY->setTickType(QValueAxis::TicksDynamic);
    axisY->setRange(g_start_y, g_end_y);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    auto transformPoint {
        [&](QPointF point) {
            point.setY(processed_image.height() - point.y());
            point.setX( (double)(point.x() - start_x) * (step_x / pps_x) );
            point.setY( (double)(point.y() - start_y) * (step_y / pps_y) );
            return point;
        }
    };

    // here goes showing graph

    /*for (GraphPoint* p : result) {
        QStack<GraphPoint*> branches;
        QLineSeries *series;
        branches.push(p);
        while(branches.length() > 0) {
            series = new QLineSeries();
            GraphPoint* cur_p = branches.pop();
            QList<GraphPoint*> next_points = cur_p->getNext();
            if (next_points.length() == 1) {
                do {
                    *series << transformPoint(cur_p->point());
                    cur_p = next_points[0];
                } while ((next_points = cur_p->getNext()).length() == 1);
                chart->addSeries(series);
                series->attachAxis(axisX);
                series->attachAxis(axisY);
            }
            if (next_points.length() > 1) {
                for (int i = 0; i < next_points.length(); i++) {
                    branches.push(next_points[i]);
                }
            }
        }
    }*/
    for (GraphPoint* p : result) {
            QStack<GraphPoint*> branches;
            QLineSeries *series;
            branches.push(p);
            while(branches.length() > 0) {
                GraphPoint* cur_p = branches.pop();
                QList<GraphPoint*> next_points = cur_p->getNext();
                if (next_points.length() > 1) {
                    for (int i = 0; i < next_points.length(); i++) {
                        series = new QLineSeries();
                        *series << transformPoint(cur_p->point());
                        GraphPoint* cur_p2 = next_points[i];
                        QList<GraphPoint*> next_points2 = cur_p2->getNext();
                        while ((next_points2 = cur_p2->getNext()).length() == 1) {
                            *series << transformPoint(cur_p2->point());
                            cur_p2 = next_points2[0];
                        }
                        *series << transformPoint(cur_p2->point());
                        if (next_points2.length() > 1)
                            branches.push(cur_p2);
                        chart->addSeries(series);
                        series->attachAxis(axisX);
                        series->attachAxis(axisY);
                    }
                }
                if (next_points.length() == 1) {
                    series = new QLineSeries();
                    do {
                        *series << transformPoint(cur_p->point());
                        cur_p = next_points[0];
                    } while ((next_points = cur_p->getNext()).length() == 1);
                    *series << transformPoint(cur_p->point());
                    if (next_points.length() > 1)
                        branches.push(cur_p);
                    chart->addSeries(series);
                    series->attachAxis(axisX);
                    series->attachAxis(axisY);
                }
            }
        }

    for (GraphPoint* p : result) {
        delete p;
    }

    this->ui->graphicsViewGraph->setChart(chart);
    emit endProcessGraph();
}

void MainWindow::onProcessGraph() {
    if (! processed_image.isNull()) {
        if (ui->comboBoxGraphMode->currentIndex() == 0) {
            emit startProcessGraph(processed_image);
        }
        else if (ui->comboBoxGraphMode->currentIndex() == 1) {
            emit startProcessGraph2(processed_image);
        }
    }
}

void MainWindow::onProcessAll() {
    connect(this, SIGNAL(endProcessImage()), this, SLOT(onProcessGraph()));
    connect(this, SIGNAL(endProcessGraph()), this, SLOT(onProcessAllEnd()));

    onProcessImage();
}

void MainWindow::onProcessAllEnd() {
    disconnect(this, SIGNAL(endProcessImage()), this, SLOT(onProcessGraph()));
    disconnect(this, SIGNAL(endProcessGraph()), this, SLOT(onProcessAllEnd()));
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
