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
