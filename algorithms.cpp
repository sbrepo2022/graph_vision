#include "algorithms.h"

QImage ImageAlgorithms::convolving(const QImage &image, double **matrix, int size) {
    QImage result(image);

    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            double weight_sum = 0;
            double matrix_sum_r = 0, matrix_sum_g = 0, matrix_sum_b = 0;
            for (int j = 0; j < size; j++) {
                for (int i = 0; i < size; i++) {
                    if (x + (i - size / 2) >= 0 && x + (i - size / 2) < image.width() && y + (j - size / 2) >= 0 && y + (j - size / 2) < image.height()) {
                        QColor pixel = image.pixelColor(x + (i - size / 2), y + (j - size / 2));
                        matrix_sum_r += pixel.red() * matrix[j][i];
                        matrix_sum_g += pixel.green() * matrix[j][i];
                        matrix_sum_b += pixel.blue() * matrix[j][i];
                        weight_sum += matrix[j][i];
                    }
                }
            }
            result.setPixelColor(x, y, QColor(matrix_sum_r / weight_sum, matrix_sum_g / weight_sum, matrix_sum_b / weight_sum));
        }
    }

    return result;
}

void ImageAlgorithms::convolving(double **val, int width, int height, const QImage &image, double *matrix, int size) {
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            double matrix_sum_r = 0, matrix_sum_g = 0, matrix_sum_b = 0;
            if (x > size / 2 && y > size / 2 && x < image.width() - size / 2 && y < image.height() - size / 2) {
                for (int j = 0; j < size; j++) {
                    for (int i = 0; i < size; i++) {
                        QColor pixel = image.pixelColor(x + (i - size / 2), y + (j - size / 2));
                        matrix_sum_r += pixel.red() * matrix[j * size + i];
                        matrix_sum_g += pixel.green() * matrix[j * size + i];
                        matrix_sum_b += pixel.blue() * matrix[j * size + i];
                    }
                }
            }
            if (x < width && y < height)
                val[y][x] = (double)(matrix_sum_r + matrix_sum_g + matrix_sum_b) / 3;
        }
    }
}


double MathFunctions::gaussian2d(double x, double y, double sigma) {
    return 1 / (2 * M_PI * sigma * sigma) * exp(-(x * x + y * y) / (2 * sigma * sigma));
}
