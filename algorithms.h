#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <cmath>
#include <algorithm>
#include <QImage>

class GradientVector {
private:
    QPoint rot;
    QPointF vec;

public:
    QPoint getRotation() { return rot; }
    QPointF getVec() { return vec; }
    void set(double x, double y) {
        vec.setX(x);
        vec.setY(y);
        rot.setX(fabs(x) > std::max(x, y) * 0.3827 ? (x < 0 ? -1 : 1) : 0);
        rot.setY(fabs(y) > std::max(x, y) * 0.3827 ? (y < 0 ? -1 : 1) : 0);
    }
    double len() { return fabs(vec.x()) + fabs(vec.y()); }
};

namespace ImageAlgorithms {
    QImage convolving(const QImage &image, double **matrix, int size);
    void convolving(double **val, int width, int height, const QImage &image, double *matrix, int size);
}

namespace MathFunctions {
    double gaussian2d(double x, double y, double sigma);
}

#endif // ALGORITHMS_H
