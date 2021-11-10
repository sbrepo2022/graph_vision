#include "imagepreprocess.h"

// ImagePreprocess
void ImagePreprocess::generateWidget(const QList<QMap<QString, QVariant>> &widget_properties) {
    interface_widget = new QGroupBox(group_name);
    QVBoxLayout *widget_layout = new QVBoxLayout(interface_widget);
    widget_layout->setContentsMargins(0, 0, 0, 0);
    QFrame *tools_frame = new QFrame(interface_widget), *body_frame = new QFrame(interface_widget);
    QHBoxLayout *tools_layout = new QHBoxLayout(tools_frame);
    QFormLayout *layout = new QFormLayout(body_frame);

    widget_layout->addWidget(tools_frame);
    widget_layout->addWidget(body_frame);

    // toolbar
    QCheckBox *use_checkbox = new QCheckBox("use");
    use_checkbox->setChecked(true);
    tools_layout->addWidget(use_checkbox);
    connect(use_checkbox, &QCheckBox::stateChanged, this, &ImagePreprocess::useFilter);

    QToolButton *top_button = new QToolButton(interface_widget);
    top_button->setText("↑");
    tools_layout->addWidget(top_button);
    connect(top_button, &QAbstractButton::clicked, this, &ImagePreprocess::moveUpPreprocess);

    QToolButton *bottom_button = new QToolButton(interface_widget);
    bottom_button->setText("↓");
    tools_layout->addWidget(bottom_button);
    connect(bottom_button, &QAbstractButton::clicked, this, &ImagePreprocess::moveDownPreprocess);

    QToolButton *delete_button = new QToolButton(interface_widget);
    delete_button->setText("✕");
    tools_layout->addWidget(delete_button);
    connect(delete_button, &QAbstractButton::clicked, this, &ImagePreprocess::deletePreprocess);

    FormGenerator::generateWidget(interface_widget, layout, widget_properties);
}

ImagePreprocess::ImagePreprocess(QObject *parent) : FormGenerator(parent), interface_widget(nullptr), use(true) {}

ImagePreprocess::~ImagePreprocess() {
    if (interface_widget != nullptr)
        delete interface_widget;
}

void ImagePreprocess::useFilter(bool use) {
    this->use = use;
}


// MonochromeGradientImage
MonochromeGradientImage::MonochromeGradientImage(QObject *parent) : ImagePreprocess(parent) {
    m_threshold = 10;

    group_name = "Monochrome gradient";
    generateWidget(QList<QMap<QString, QVariant>>(
    {
        {
            std::pair<QString, QVariant>("name", "threshold"),
            std::pair<QString, QVariant>("min", 0),
            std::pair<QString, QVariant>("max", 255)
        }
    }));
}

QImage MonochromeGradientImage::processImage(const QImage &image) {
    int step = 1;

    QImage result(image);
    for (int y = 0; y < image.height() - step; y++) {
        for (int x = 0; x < image.width() - step; x++) {

            QColor pixel = image.pixelColor(x, y);
            QColor pixel_x = image.pixelColor(x + step, y);
            QColor pixel_y = image.pixelColor(x, y + step);
            int avg = (pixel.red() + pixel.green() + pixel.blue()) / 3;
            int avg_x = (pixel_x.red() + pixel_x.green() + pixel_x.blue()) / 3;
            int avg_y = (pixel_y.red() + pixel_y.green() + pixel_y.blue()) / 3;
            int gradient = sqrt((avg_x - avg) * (avg_x - avg) + (avg_y - avg) * (avg_y - avg));

            int set_avg;
            if (gradient >= m_threshold) {
                set_avg = 0;
            }
            else {
                set_avg = 255;
            }
            result.setPixelColor(x, y, QColor(set_avg, set_avg, set_avg));
        }
    }
    return result;
}

MonochromeGradientImage::~MonochromeGradientImage() {}


// GaussianBlur
GaussianBlur::GaussianBlur(QObject *parent) : ImagePreprocess(parent) {
    m_sigma = 1.4f;
    m_size = 5;

    group_name = "Gaussian blur";
    generateWidget(QList<QMap<QString, QVariant>>(
    {
        {
            std::pair<QString, QVariant>("name", "sigma")
        },
        {
            std::pair<QString, QVariant>("name", "size")
        }
    }));
}

QImage GaussianBlur::processImage(const QImage &image) {
    double **matrix;

    matrix = new double*[m_size];
    for (int i = 0; i < m_size; i++) {
        matrix[i] = new double[m_size];
    }

    for (int j = 0; j < m_size; j++) {
        for (int i = 0; i < m_size; i++) {
            matrix[j][i] = MathFunctions::gaussian2d(i - m_size / 2, j - m_size / 2, m_sigma);
        }
    }

    QImage result = ImageAlgorithms::convolving(image, matrix, m_size);

    for (int i = 0; i < m_size; i++) {
        delete [] matrix[i];
    }
    delete [] matrix;

    return result;
}

GaussianBlur::~GaussianBlur() {}


// CannyFilter

CannyFilter::CannyFilter(QObject *parent) : ImagePreprocess(parent) {
    m_supression = true;
    m_threshold_low = 15;
    m_threshold_high = 20;
    m_hysteresis = true;

    group_name = "Canny filter";
    generateWidget(QList<QMap<QString, QVariant>>(
    {
        {
            std::pair<QString, QVariant>("name", "filter"),
            std::pair<QString, QVariant>("field_type", "list"),
            std::pair<QString, QVariant>("variants", QStringList({"prewitt_3", "prewitt_5", "prewitt_7", "sobel", "tpo_5", "tpo_7"}))
        },
        {
            std::pair<QString, QVariant>("name", "supression")
        },
        {
            std::pair<QString, QVariant>("name", "threshold_low"),
            std::pair<QString, QVariant>("min", 0),
            std::pair<QString, QVariant>("max", 255)
        },
        {
            std::pair<QString, QVariant>("name", "threshold_high"),
            std::pair<QString, QVariant>("min", 0),
            std::pair<QString, QVariant>("max", 255)
        },
        {
            std::pair<QString, QVariant>("name", "hysteresis")
        }
    }));
}

QImage CannyFilter::processImage(const QImage &image) {
    QImage result(image);

    double matrix_x_prewitt_3[9] = {
        -1, 0, 1,
        -1, 0, 1,
        -1, 0, 1
    };

    double matrix_y_prewitt_3[9] = {
        -1, -1, -1,
        0, 0, 0,
        1, 1, 1
    };

    double matrix_x_prewitt_5[25] = {
        -1, -1, 0, 1, 1,
        -1, -1, 0, 1, 1,
        -1, -1, 0, 1, 1,
        -1, -1, 0, 1, 1,
        -1, -1, 0, 1, 1,
    };

    double matrix_y_prewitt_5[25] = {
        -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1,
         0,  0,  0,  0,  0,
         1,  1,  1,  1,  1,
         1,  1,  1,  1,  1
    };

    double matrix_x_prewitt_7[49] = {
        -1, -1, -1, 0, 1, 1, 1,
        -1, -1, -1, 0, 1, 1, 1,
        -1, -1, -1, 0, 1, 1, 1,
        -1, -1, -1, 0, 1, 1, 1,
        -1, -1, -1, 0, 1, 1, 1,
        -1, -1, -1, 0, 1, 1, 1,
    };

    double matrix_y_prewitt_7[49] = {
        -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1,
         0,  0,  0,  0,  0,  0,  0,
         1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1
    };

    double matrix_x_sobel[9] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };

    double matrix_y_sobel[9] = {
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1
    };

    double matrix_x_tpo_5[25] = {
        -1, -1, 0, 1, 1,
        -1, -2, 0, 2, 1,
        -1, -3, 0, 3, 1,
        -1, -2, 0, 2, 1,
        -1, -1, 0, 1, 1
    };

    double matrix_y_tpo_5[25] = {
        -1, -1, -1, -1, -1,
        -1, -2, -3, -2, -1,
         0,  0,  0,  0,  0,
         1,  2,  3,  2,  1,
         1,  1,  1,  1,  1
    };

    double matrix_x_tpo_7[49] = {
        -1, -1, -1, 0, 1, 1, 1,
        -1, -2, -2, 0, 2, 2, 1,
        -1, -2, -3, 0, 3, 2, 1,
        -1, -2, -3, 0, 3, 2, 1,
        -1, -2, -3, 0, 3, 2, 1,
        -1, -2, -2, 0, 2, 2, 1,
        -1, -1, -1, 0, 1, 1, 1
    };

    double matrix_y_tpo_7[49] = {
        -1, -1, -1, -1, -1, -1, -1,
        -1, -2, -2, -2, -2, -2, -1,
        -1, -2, -3, -3, -3, -2, -1,
         0,  0,  0,  0,  0,  0,  0,
         1,  2,  3,  3,  3,  2,  1,
         1,  2,  2,  2,  2,  2,  1,
         1,  1,  1,  1,  1,  1,  1
    };

    double *matrix_x = matrix_x_prewitt_3, *matrix_y = matrix_y_prewitt_3;
    int matrix_size = 3, weight_divider = 3;

    if (m_filter == "prewitt_3") {
        matrix_x = matrix_x_prewitt_3;
        matrix_y = matrix_y_prewitt_3;
        matrix_size = 3;
        weight_divider = 3;
    }
    if (m_filter == "prewitt_5") {
        matrix_x = matrix_x_prewitt_5;
        matrix_y = matrix_y_prewitt_5;
        matrix_size = 5;
        weight_divider = 10;
    }
    if (m_filter == "prewitt_7") {
        matrix_x = matrix_x_prewitt_7;
        matrix_y = matrix_y_prewitt_7;
        matrix_size = 7;
        weight_divider = 21;
    }
    else if (m_filter == "sobel") {
        matrix_x = matrix_x_sobel;
        matrix_y = matrix_y_sobel;
        matrix_size = 3;
        weight_divider = 4;
    }
    else if (m_filter == "tpo_5") {
        matrix_x = matrix_x_tpo_5;
        matrix_y = matrix_y_tpo_5;
        matrix_size = 5;
        weight_divider = 15;
    }
    else if (m_filter == "tpo_7") {
        matrix_x = matrix_x_tpo_7;
        matrix_y = matrix_y_tpo_7;
        matrix_size = 7;
        weight_divider = 34;
    }

    // init
    double **values_x, **values_y;
    GradientVector **gf_1, **gf_2;
    int **dtf, **dtf2;
    values_x = new double*[image.height()];
    values_y = new double*[image.height()];
    gf_1 = new GradientVector*[image.height()];
    gf_2 = new GradientVector*[image.height()];
    dtf = new int*[image.height()];
    dtf2 = new int*[image.height()];
    for (int i = 0; i < image.height(); i++) {
        values_x[i] = new double[image.width()];
        values_y[i] = new double[image.width()];
        gf_1[i] = new GradientVector[image.width()];
        gf_2[i] = new GradientVector[image.width()];
        dtf[i] = new int[image.width()];
        dtf2[i] = new int[image.width()];
    }

    // find gradient
    ImageAlgorithms::convolving(values_x, image.width(), image.height(), image, matrix_x, matrix_size);
    ImageAlgorithms::convolving(values_y, image.width(), image.height(), image, matrix_y, matrix_size);

    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            gf_1[j][i].set(values_x[j][i], values_y[j][i]);
            gf_2[j][i] = gf_1[j][i];
        }
    }

    // Gradient magnitude thresholding or lower bound cut-off suppression
    if (m_supression) {
        for (int j = 1; j < image.height() - 1; j++) {
            for (int i = 1; i < image.width() - 1; i++) {
                GradientVector gv0, gv1, gv2;
                gv0 = gf_1[j][i];
                gv1 = gf_1[j + gv0.getRotation().y()][i + gv0.getRotation().x()];
                gv2 = gf_1[j - gv0.getRotation().y()][i - gv0.getRotation().x()];
                if (! (gv0.len() > gv1.len() && gv0.len() > gv2.len())) {
                    gf_2[j][i].set(0, 0);
                }
            }
        }
    }

    // Double threshold
    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            dtf[j][i] = gf_2[j][i].len() > m_threshold_low * weight_divider ? (gf_2[j][i].len() < m_threshold_high * weight_divider ? 1 : 2) : 0;
        }
    }


    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            dtf2[j][i] = dtf[j][i];
        }
    }
    // Edge tracking by hysteresis
    if (m_hysteresis) {
        for (int j = 1; j < image.height() - 1; j++) {
            for (int i = 1; i < image.width() - 1; i++) {
                //result.setPixelColor(i, j, QColor(127 * dtf[j][i], 127 * dtf[j][i], 127 * dtf[j][i]));
                if (dtf[j][i] == 1) {
                    bool found = false;
                    for (int y = j - 1; y <= j + 1; y++) {
                        for (int x = i - 1; x <= i + 1; x++) {
                            if (!(x == i && y == j) && dtf[y][x] == 2)
                                found = true;
                        }
                    }
                    if (! found)
                        dtf2[j][i] = 0;
                }
            }
        }
    }

    // monochromize
    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            result.setPixelColor(i, j, QColor(dtf2[j][i] != 0 ? (dtf2[j][i] == 1 ? 128 : 255) : 0, dtf2[j][i] != 0 ? (dtf2[j][i] == 1 ? 128 : 255) : 0, dtf2[j][i] != 0 ? (dtf2[j][i] == 1 ? 128 : 255) : 0));
        }
    }

    // destruct
    for (int i = 0; i < image.height(); i++) {
        delete [] values_x[i];
        delete [] values_y[i];
        delete [] gf_1[i];
        delete [] gf_2[i];
        delete [] dtf[i];
        delete [] dtf2[i];
    }
    delete [] values_x;
    delete [] values_y;
    delete [] gf_1;
    delete [] gf_2;
    delete [] dtf;
    delete [] dtf2;

    return result;
}

CannyFilter::~CannyFilter() {}


// ColorGradientField
ColorGradientField::ColorGradientField(QObject *parent) : ImagePreprocess(parent) {
    m_grayscale = true;

    group_name = "Color gradient filter";
    generateWidget(QList<QMap<QString, QVariant>>(
    {
        {
            std::pair<QString, QVariant>("name", "filter"),
            std::pair<QString, QVariant>("field_type", "list"),
            std::pair<QString, QVariant>("variants", QStringList({"prewitt_3", "prewitt_5", "prewitt_7", "sobel", "tpo_5", "tpo_7"}))
        },
        {
            std::pair<QString, QVariant>("name", "grayscale")
        }
    }));
}

QImage ColorGradientField::processImage(const QImage &image) {
    QImage result(image);

    double matrix_x_prewitt_3[9] = {
        -1, 0, 1,
        -1, 0, 1,
        -1, 0, 1
    };

    double matrix_y_prewitt_3[9] = {
        -1, -1, -1,
        0, 0, 0,
        1, 1, 1
    };

    double matrix_x_prewitt_5[25] = {
        -1, -1, 0, 1, 1,
        -1, -1, 0, 1, 1,
        -1, -1, 0, 1, 1,
        -1, -1, 0, 1, 1,
        -1, -1, 0, 1, 1,
    };

    double matrix_y_prewitt_5[25] = {
        -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1,
         0,  0,  0,  0,  0,
         1,  1,  1,  1,  1,
         1,  1,  1,  1,  1
    };

    double matrix_x_prewitt_7[49] = {
        -1, -1, -1, 0, 1, 1, 1,
        -1, -1, -1, 0, 1, 1, 1,
        -1, -1, -1, 0, 1, 1, 1,
        -1, -1, -1, 0, 1, 1, 1,
        -1, -1, -1, 0, 1, 1, 1,
        -1, -1, -1, 0, 1, 1, 1,
    };

    double matrix_y_prewitt_7[49] = {
        -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1,
         0,  0,  0,  0,  0,  0,  0,
         1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1
    };

    double matrix_x_sobel[9] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };

    double matrix_y_sobel[9] = {
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1
    };

    double matrix_x_tpo_5[25] = {
        -1, -1, 0, 1, 1,
        -1, -2, 0, 2, 1,
        -1, -3, 0, 3, 1,
        -1, -2, 0, 2, 1,
        -1, -1, 0, 1, 1
    };

    double matrix_y_tpo_5[25] = {
        -1, -1, -1, -1, -1,
        -1, -2, -3, -2, -1,
         0,  0,  0,  0,  0,
         1,  2,  3,  2,  1,
         1,  1,  1,  1,  1
    };

    double matrix_x_tpo_7[49] = {
        -1, -1, -1, 0, 1, 1, 1,
        -1, -2, -2, 0, 2, 2, 1,
        -1, -2, -3, 0, 3, 2, 1,
        -1, -2, -3, 0, 3, 2, 1,
        -1, -2, -3, 0, 3, 2, 1,
        -1, -2, -2, 0, 2, 2, 1,
        -1, -1, -1, 0, 1, 1, 1
    };

    double matrix_y_tpo_7[49] = {
        -1, -1, -1, -1, -1, -1, -1,
        -1, -2, -2, -2, -2, -2, -1,
        -1, -2, -3, -3, -3, -2, -1,
         0,  0,  0,  0,  0,  0,  0,
         1,  2,  3,  3,  3,  2,  1,
         1,  2,  2,  2,  2,  2,  1,
         1,  1,  1,  1,  1,  1,  1
    };

    double *matrix_x = matrix_x_prewitt_3, *matrix_y = matrix_y_prewitt_3;
    int matrix_size = 3;

    if (m_filter == "prewitt_3") {
        matrix_x = matrix_x_prewitt_3;
        matrix_y = matrix_y_prewitt_3;
        matrix_size = 3;
    }
    if (m_filter == "prewitt_5") {
        matrix_x = matrix_x_prewitt_5;
        matrix_y = matrix_y_prewitt_5;
        matrix_size = 5;
    }
    if (m_filter == "prewitt_7") {
        matrix_x = matrix_x_prewitt_7;
        matrix_y = matrix_y_prewitt_7;
        matrix_size = 7;
    }
    else if (m_filter == "sobel") {
        matrix_x = matrix_x_sobel;
        matrix_y = matrix_y_sobel;
        matrix_size = 3;
    }
    else if (m_filter == "tpo_5") {
        matrix_x = matrix_x_tpo_5;
        matrix_y = matrix_y_tpo_5;
        matrix_size = 5;
    }
    else if (m_filter == "tpo_7") {
        matrix_x = matrix_x_tpo_7;
        matrix_y = matrix_y_tpo_7;
        matrix_size = 7;
    }

    // init
    double min_len = -1, max_len = -1, cur_len = -1;

    double **values_x, **values_y;
    GradientVector **gf_1;
    values_x = new double*[image.height()];
    values_y = new double*[image.height()];
    gf_1 = new GradientVector*[image.height()];
    for (int i = 0; i < image.height(); i++) {
        values_x[i] = new double[image.width()];
        values_y[i] = new double[image.width()];
        gf_1[i] = new GradientVector[image.width()];
    }

    // find gradient
    ImageAlgorithms::convolving(values_x, image.width(), image.height(), image, matrix_x, matrix_size);
    ImageAlgorithms::convolving(values_y, image.width(), image.height(), image, matrix_y, matrix_size);

    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            gf_1[j][i].set(values_x[j][i], values_y[j][i]);
            cur_len = gf_1[j][i].len();
            if (min_len < 0 || max_len < 0) {
                min_len = max_len = cur_len;
            }
            if (min_len > cur_len) {
                min_len = cur_len;
            }
            if (max_len < cur_len) {
                max_len = cur_len;
            }
        }
    }

    // colorize
    double cur_min, cur_max;
    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            cur_len = gf_1[j][i].len();
            if (! m_grayscale) {
                if (cur_len < (max_len - min_len) * 0.5) {
                    cur_min = min_len;
                    cur_max = (max_len - min_len) * 0.5;
                    result.setPixelColor(i, j, QColor( 0, (cur_len - cur_min) * 255 / (cur_max - cur_min), 255 + (cur_len - cur_min) * -255 / (cur_max - cur_min) ));
                }
                else {
                    cur_min = (max_len - min_len) * 0.5;
                    cur_max = max_len;
                    result.setPixelColor(i, j, QColor( (cur_len - cur_min) * 255 / (cur_max - cur_min), 255 + (cur_len - cur_min) * -255 / (cur_max - cur_min), 0 ));
                }
            }
            else {
                int val = (cur_len - min_len) * 255 / (max_len - min_len);
                result.setPixelColor(i, j, QColor(val, val, val));
            }
        }
    }

    // destruct
    for (int i = 0; i < image.height(); i++) {
        delete [] values_x[i];
        delete [] values_y[i];
        delete [] gf_1[i];
    }
    delete [] values_x;
    delete [] values_y;
    delete [] gf_1;

    return result;
}

ColorGradientField::~ColorGradientField() {}



// SegmentationField
SegmentationField::SegmentationField(QObject *parent) : ImagePreprocess(parent) {
    m_threshold_low = 40;
    m_classification = true;
    m_autoclassification = true;
    m_classification_threshold = 100;

    group_name = "Sementation filter";
    generateWidget(QList<QMap<QString, QVariant>>(
    {
        {
            std::pair<QString, QVariant>("name", "threshold_low"),
            std::pair<QString, QVariant>("min", 0),
            std::pair<QString, QVariant>("max", 255)
        },
        {
            std::pair<QString, QVariant>("name", "classification")
        },
        {
            std::pair<QString, QVariant>("name", "classification_threshold"),
            std::pair<QString, QVariant>("min", 0),
            std::pair<QString, QVariant>("max", 10000 * 10000)
        },
        {
            std::pair<QString, QVariant>("name", "autoclassification")
        }
    }));
}

QImage SegmentationField::processImage(const QImage &image) {
    QImage result(image);

    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            int color_val = result.pixelColor(i, j).value();
            result.setPixelColor(i, j, QColor(color_val));
            if (result.pixelColor(i, j).value() < m_threshold_low) {
                result.setPixelColor(i, j, QColor(0, 0, 0));
            }
        }
    }

    int cur_area = 1;
    int **areas_field;
    areas_field = new int*[image.height()];
    for (int i = 0; i < image.height(); i++) {
        areas_field[i] = new int[image.width()];
        for (int j = 0; j < image.width(); j++) {
            areas_field[i][j] = -1;
        }
    }

    auto fillArea {
        [&](int i, int j, int cur_area) {
            QStack<QPoint> stack;
            stack.push(QPoint(i, j));
            while (! stack.isEmpty()) {
                QPoint cur_p = stack.pop();
                areas_field[cur_p.y()][cur_p.x()] = cur_area;
                if (cur_p.x() + 1 < image.width() && result.pixelColor(cur_p.x() + 1, cur_p.y()).value() >= result.pixelColor(cur_p.x(), cur_p.y()).value() && areas_field[cur_p.y()][cur_p.x() + 1] == -1) {
                    QPoint new_p(cur_p.x() + 1, cur_p.y());
                    areas_field[new_p.y()][new_p.x()] = 0;
                    stack.push(new_p);
                }
                if (cur_p.x() - 1 >= 0 && result.pixelColor(cur_p.x() - 1, cur_p.y()).value() >= result.pixelColor(cur_p.x(), cur_p.y()).value() && areas_field[cur_p.y()][cur_p.x() - 1] == -1) {
                    QPoint new_p(cur_p.x() - 1, cur_p.y());
                    areas_field[new_p.y()][new_p.x()] = 0;
                    stack.push(new_p);
                }
                if (cur_p.y() + 1 < image.height() && result.pixelColor(cur_p.x(), cur_p.y() + 1).value() >= result.pixelColor(cur_p.x(), cur_p.y()).value() && areas_field[cur_p.y() + 1][cur_p.x()] == -1) {
                    QPoint new_p(cur_p.x(), cur_p.y() + 1);
                    areas_field[new_p.y()][new_p.x()] = 0;
                    stack.push(new_p);
                }
                if (cur_p.y() - 1 >= 0 && result.pixelColor(cur_p.x(), cur_p.y() - 1).value() >= result.pixelColor(cur_p.x(), cur_p.y()).value() && areas_field[cur_p.y() - 1][cur_p.x()] == -1) {
                    QPoint new_p(cur_p.x(), cur_p.y() - 1);
                    areas_field[new_p.y()][new_p.x()] = 0;
                    stack.push(new_p);
                }
            }
        }
    };

    // fill areas minimums
    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            if (result.pixelColor(i, j).value() == 0 && areas_field[j][i] == -1) {
                fillArea(i, j, cur_area);
                cur_area++;
            }
        }
    }

    // divide for big and small groups
    if (m_classification) {
        QMap<int, int> areas_squares;
        for (int j = 0; j < image.height(); j++) {
            for (int i = 0; i < image.width(); i++) {
                if (areas_field[j][i] > 0) {
                    if (areas_squares.contains(areas_field[j][i])) areas_squares[areas_field[j][i]]++;
                    else areas_squares.insert(areas_field[j][i], 1);
                }
            }
        }

        int classification_threshold =  (double)(image.width() * image.height()) / areas_squares.count();

        for (int j = 0; j < image.height(); j++) {
            for (int i = 0; i < image.width(); i++) {
                if (areas_field[j][i] > 0) {
                    if (areas_squares[areas_field[j][i]] < (m_autoclassification ? classification_threshold : m_classification_threshold)) areas_field[j][i] = 2;
                    else areas_field[j][i] = 1;
                }
            }
        }
    }

    // delete holes
    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            if (areas_field[j][i] <= 0) {
                QPoint p1(i, j), p2(i, j);
                bool fix_l = false, fix_r = false, fix_t = false, fix_b = false;
                QMap<int, int> near_areas;
                while (! fix_l || ! fix_r || ! fix_t || ! fix_b) {
                    if (p2.x() + 1 < image.width() && ! fix_r) {
                        p2.setX(p2.x() + 1);
                        int count = 0;
                        for (int y = p1.y(); y <= p2.y(); y++) {
                            if (areas_field[y][p2.x()] <= 0)
                                count++;
                            else {
                                if (areas_field[y][p2.x() - 1] <= 0) {
                                    if (near_areas.contains(areas_field[y][p2.x()])) near_areas[areas_field[y][p2.x()]]++;
                                    else near_areas.insert(areas_field[y][p2.x()], 1);
                                }
                            }
                        }
                        if (count == 0) fix_r = true;
                    }
                    else fix_r = true;

                    if (p2.y() + 1 < image.height() && ! fix_b) {
                        p2.setY(p2.y() + 1);
                        int count = 0;
                        for (int x = p1.x(); x <= p2.x(); x++) {
                            if (areas_field[p2.y()][x] <= 0)
                                count++;
                            else {
                                if (areas_field[p2.y() - 1][x] <= 0) {
                                    if (near_areas.contains(areas_field[p2.y()][x])) near_areas[areas_field[p2.y()][x]]++;
                                    else near_areas.insert(areas_field[p2.y()][x], 1);
                                }
                            }
                        }
                        if (count == 0) fix_b = true;
                    }
                    else fix_b = true;

                    if (p1.x() - 1 >= 0 && ! fix_l) {
                        p1.setX(p1.x() - 1);
                        int count = 0;
                        for (int y = p1.y(); y <= p2.y(); y++) {
                            if (areas_field[y][p1.x()] <= 0)
                                count++;
                            else {
                                if (areas_field[y][p1.x() + 1] <= 0) {
                                    if (near_areas.contains(areas_field[y][p1.x()])) near_areas[areas_field[y][p1.x()]]++;
                                    else near_areas.insert(areas_field[y][p1.x()], 1);
                                }
                            }
                        }
                        if (count == 0) fix_l = true;
                    }
                    else fix_l = true;

                    if (p1.y() - 1 >= 0 && ! fix_t) {
                        p1.setY(p1.y() - 1);
                        int count = 0;
                        for (int x = p1.x(); x <= p2.x(); x++) {
                            if (areas_field[p1.y()][x] <= 0)
                                count++;
                            else {
                                if (areas_field[p1.y() + 1][x] <= 0) {
                                    if (near_areas.contains(areas_field[p1.y()][x])) near_areas[areas_field[p1.y()][x]]++;
                                    else near_areas.insert(areas_field[p1.y()][x], 1);
                                }
                            }
                        }
                        if (count == 0) fix_t = true;
                    }
                    else fix_t = true;

                }

                int max_area = -1;
                int max_area_val = -1;
                for (auto it = near_areas.begin(); it != near_areas.end(); it++) {
                    if (it.value() > max_area_val) {
                        max_area_val = it.value();
                        max_area = it.key();
                    }
                }

                //qDebug() << p2 - p1;

                for (int x = p1.x(); x <= p2.x(); x++) {
                    for (int y = p1.y(); y <= p2.y(); y++) {
                        if (areas_field[y][x] <= 0) areas_field[y][x] = max_area;
                    }
                }
            }
        }
    }

    // colorize
    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            if (areas_field[j][i] > 0) {
                if (! m_classification) {
                    QColor area_color = QColor::fromHsvF(1.0 / (double)cur_area * (double)areas_field[j][i], 1.0, 1.0);
                    result.setPixelColor(i, j, area_color);
                }
                else {
                    if (areas_field[j][i] == 1) {
                        result.setPixelColor(i, j, QColor(0, 0, 0));
                    }
                    else if (areas_field[j][i] == 2) {
                        result.setPixelColor(i, j, QColor(255, 255, 255));
                    }
                }
            }
        }
    }

    for (int i = 0; i < image.height(); i++) {
        delete [] areas_field[i];
    }
    delete [] areas_field;

    return result;
}

SegmentationField::~SegmentationField() {}



ThinningFilter::ThinningFilter(QObject *parent) : ImagePreprocess(parent) {
    group_name = "Thinning filter";
    generateWidget(QList<QMap<QString, QVariant>>());
}

QImage ThinningFilter::processImage(const QImage &image) {
    QImage result(image);

    int **res = new int*[image.height()];
    int **buf = new int*[image.height()];
    for (int i = 0; i < image.height(); i++) {
        res[i] = new int[image.width()];
        buf[i] = new int[image.width()];
    }

    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            buf[j][i] = image.pixelColor(i, j).value() < 127 ? 0 : 1;
            if (i == 0 || j == 0 || i == image.width() - 1 || j == image.height() - 1) buf[j][i] = 0;
        }
    }

    auto copyBuf {
        [&]() {
            for (int j = 0; j < image.height(); j++) {
                for (int i = 0; i < image.width(); i++) {
                    res[j][i] = buf[j][i];
                }
            }
        }
    };

    auto S {
        [](int x, int y, int **res) {
            int N[9] = {res[y-1][x], res[y-1][x+1], res[y][x+1], res[y+1][x+1], res[y+1][x], res[y+1][x-1], res[y][x-1], res[y-1][x-1], res[y-1][x]};
            int count = 0;
            int last = N[0];
            for (int i = 1; i < 9; i++) {
                if (N[i] == 1 && last == 0) count++;
                last = N[i];
            }
            return count;
        }
    };

    copyBuf();
    bool found = true;
    while (found) {
        found = false;
        for (int j = 1; j < image.height() - 1; j++) {
            for (int i = 1; i < image.width() - 1; i++) {
                if (res[j][i] == 1) {
                    if (2 <= res[j-1][i] + res[j-1][i+1] + res[j][i+1] + res[j+1][i+1] + res[j+1][i] + res[j+1][i-1] + res[j][i-1] + res[j-1][i-1]
                        && res[j-1][i] + res[j-1][i+1] + res[j][i+1] + res[j+1][i+1] + res[j+1][i] + res[j+1][i-1] + res[j][i-1] + res[j-1][i-1] <= 6
                        && S(i, j, res) == 1
                        && ((res[j-1][i]*res[j][i+1]*res[j+1][i] == 0 && res[j][i+1]*res[j+1][i]*res[j][i-1] == 0) || (res[j-1][i]*res[j][i+1]*res[j][i-1] == 0 && res[j-1][i]*res[j+1][i]*res[j][i-1] == 0))) {
                        if (S(i, j, buf) == 1) {
                            buf[j][i] = 0;
                            found = true;
                        }
                    }
                }
            }
        }
        copyBuf();
    }

    for (int j = 0; j < image.height(); j++) {
        for (int i = 0; i < image.width(); i++) {
            result.setPixelColor(i, j, QColor(res[j][i] ? 255 : 0, res[j][i] ? 255 : 0, res[j][i] ? 255 : 0));
        }
    }

    for (int i = 0; i < image.height(); i++) {
        delete [] res[i];
        delete [] buf[i];
    }
    delete [] res;
    delete [] buf;

    return result;
}

ThinningFilter::~ThinningFilter() {}



// ImageProcessor
ImageProcessor::ImageProcessor(QWidget *prop_group_widget, QObject *parent) : QObject(parent) {
    this->prop_group_widget = prop_group_widget;
}

ImageProcessor::~ImageProcessor() {
    for (int i = 0; i < this->middleware.count(); i++) {
        delete middleware[i];
    }
}

void ImageProcessor::addMiddleware(ImagePreprocess* mid_elem) {
    this->middleware.push_back(mid_elem);
    if (mid_elem->getWidget() != nullptr) {
        prop_group_widget->layout()->addWidget(mid_elem->getWidget());
    }
    connect(mid_elem, &ImagePreprocess::moveUpPreprocess, this, &ImageProcessor::moveUpPreprocess);
    connect(mid_elem, &ImagePreprocess::moveDownPreprocess, this, &ImageProcessor::moveDownPreprocess);
    connect(mid_elem, &ImagePreprocess::deletePreprocess, this, &ImageProcessor::deletePreprocess);
}

void ImageProcessor::clear() {
    ImagePreprocess *preprocess;
    QMutableListIterator<ImagePreprocess*> iter(middleware);
    while (iter.hasNext()) {
        preprocess = iter.next();
        preprocess->deleteLater();
        iter.remove();
    }
}

// slots
void ImageProcessor::processImage(const QImage &image) {
    emit startCalculating(middleware.count(), "Processing image...");
    QImage result(image);
    for (int i = 0; i < middleware.count(); i++) {
        emit currentFilter(i, middleware[i]->getGroupName());
        if (! middleware[i]->isUse())
            continue;
        result = middleware[i]->processImage(result);
    }
    emit finishCalculating();
    emit resultReady(result);
}

void ImageProcessor::moveUpPreprocess() {
    ImagePreprocess *preprocess = qobject_cast<ImagePreprocess*>(sender());
    int widget_pos = qobject_cast<QBoxLayout*>(prop_group_widget->layout())->indexOf(preprocess->getWidget());
    QMutableListIterator<ImagePreprocess*> iter(middleware);
    iter.toBack();
    while (iter.hasPrevious()) {
        if (iter.previous() == preprocess) {
            if (iter.hasPrevious()) {
                iter.remove();
                iter.previous();
                iter.insert(preprocess);

                qobject_cast<QBoxLayout*>(prop_group_widget->layout())->removeWidget(preprocess->getWidget());
                qobject_cast<QBoxLayout*>(prop_group_widget->layout())->insertWidget(widget_pos - 1, preprocess->getWidget());
                break;
            }
        }
    }
}

void ImageProcessor::moveDownPreprocess() {
    ImagePreprocess *preprocess = qobject_cast<ImagePreprocess*>(sender());
    int widget_pos = qobject_cast<QBoxLayout*>(prop_group_widget->layout())->indexOf(preprocess->getWidget());
    QMutableListIterator<ImagePreprocess*> iter(middleware);
    iter.toFront();
    while (iter.hasNext()) {
        if (iter.next() == preprocess) {
            if (iter.hasNext()) {
                iter.remove();
                iter.next();
                iter.insert(preprocess);

                qobject_cast<QBoxLayout*>(prop_group_widget->layout())->removeWidget(preprocess->getWidget());
                qobject_cast<QBoxLayout*>(prop_group_widget->layout())->insertWidget(widget_pos + 1, preprocess->getWidget());
                break;
            }
        }
    }
}

void ImageProcessor::deletePreprocess() {
    ImagePreprocess *preprocess = qobject_cast<ImagePreprocess*>(sender());
    QMutableListIterator<ImagePreprocess*> iter(middleware);
    while (iter.hasNext()) {
        if (iter.next() == preprocess) {
            preprocess->deleteLater();
            iter.remove();
        }
    }
}
