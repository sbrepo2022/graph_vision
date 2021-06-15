#include "imagepreprocess.h"

// ImagePreprocess
ImagePreprocess::ImagePreprocess() {}

ImagePreprocess::~ImagePreprocess() {}


// MonochromeImage
MonochromeImage::MonochromeImage() : ImagePreprocess() {}

void MonochromeImage::setThreshold(int threshold) {
    this->threshold = threshold;
}

QImage MonochromeImage::processImage(const QImage &image) {
    QImage result(image);
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            QColor pixel = image.pixelColor(x, y);
            int avg = (pixel.red() + pixel.green() + pixel.blue()) / 3;
            if (avg <= threshold) {
                result.setPixelColor(x, y, QColor(0, 0, 0));
            }
            else {
                result.setPixelColor(x, y, QColor(255, 255, 255));
            }
        }
    }
    return result;
}

MonochromeImage::~MonochromeImage() {}


// MonochromeGradientImage
MonochromeGradientImage::MonochromeGradientImage() : ImagePreprocess() {
    threshold = 20;
}

void MonochromeGradientImage::setThreshold(int threshold) {
    this->threshold = threshold;
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
            if (gradient >= threshold) {
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


// ImageProcessor
ImageProcessor::ImageProcessor() {}

ImageProcessor::~ImageProcessor() {
    for (int i = 0; i < this->middleware.count(); i++) {
        delete middleware[i];
    }
}

void ImageProcessor::addMiddleware(ImagePreprocess* mid_elem) {
    this->middleware.push_back(mid_elem);
}

QImage ImageProcessor::processImage(const QImage &image) {
    QImage result(image);
    for (int i = 0; i < this->middleware.count(); i++) {
        result = middleware[i]->processImage(result);
    }
    return result;
}
