#ifndef IMAGEPREPROCESS_H
#define IMAGEPREPROCESS_H

#include <QImage>

class ImagePreprocess {
public:
    ImagePreprocess();

    virtual QImage processImage(const QImage &image) = 0;
};

class MonochromeImage : public ImagePreprocess {
private:
    float threshold;

public:
    MonochromeImage(float threshold);

    virtual QImage processImage(const QImage &image);
};

#endif // IMAGEPREPROCESS_H
