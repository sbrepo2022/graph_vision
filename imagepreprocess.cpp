#include "imagepreprocess.h"

ImagePreprocess::ImagePreprocess() {}


MonochromeImage::MonochromeImage(float threshold) : ImagePreprocess(), threshold(threshold) {}

QImage MonochromeImage::processImage(const QImage &image) {

}
