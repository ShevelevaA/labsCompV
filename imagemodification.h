#ifndef IMAGEMODIFICATION_H
#define IMAGEMODIFICATION_H
#define _USE_MATH_DEFINES

#include <QPixmap>
#include <memory>
#include <math.h>

using namespace std;

enum class Modification
{
    SobelX,
    SobelY,
    GaussX,
    GaussY
};

class ImageModification
{
private:
    int width;
    int height;
    unique_ptr<double []> matrixModification;
public:
    ImageModification(Modification modification);
    ImageModification(Modification modification, double sigma);
    int getHeight();
    int getWidth();
    double geValuetPixelModification(int coordX, int coordY);
private:
    void feelMatrixSobelX();
    void feelMatrixSobelY();
    int getDimentionRadius(double sigma);
    void feelGaussOneCoord(double sigma);

};

#endif // IMAGEMODIFICATION_H
