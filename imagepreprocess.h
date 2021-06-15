#ifndef IMAGEPREPROCESS_H
#define IMAGEPREPROCESS_H

#include <QImage>

class ImagePreprocess {
public:
    ImagePreprocess();

    virtual QImage processImage(const QImage &image) = 0;
    virtual ~ImagePreprocess();
};

class MonochromeImage : public ImagePreprocess {
private:
    int threshold;

public:
    MonochromeImage();
    void setThreshold(int threshold);

    virtual QImage processImage(const QImage &image);
    virtual ~MonochromeImage();
};

class MonochromeGradientImage : public ImagePreprocess {
private:
    int threshold;

public:
    MonochromeGradientImage();
    void setThreshold(int threshold);

    virtual QImage processImage(const QImage &image);
    virtual ~MonochromeGradientImage();
};

class ImageProcessor {
private:
    QList<ImagePreprocess*> middleware;

public:
    ImageProcessor();
    ~ImageProcessor();

    void addMiddleware(ImagePreprocess* mid_elem);
    QImage processImage(const QImage &image);
};

#endif // IMAGEPREPROCESS_H
