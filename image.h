#ifndef IMAGE_H
#define IMAGE_H

#include "imagemodification.h"

#include <QPixmap>
#include <memory>

using namespace std;

enum class Border
{
    Black,
    Copy,
    Reflection,
    Turn
};

class Image
{
private:
    unique_ptr<double []> matrix;
    int width;
    int height;
public:    
    Image(int width, int height);
    Image(const Image &image);
    static Image *getSobelXY(Image *imgX, Image *imgY);

    int getWidth();
    int getHeight();
    QPixmap getImage();
    void changeImage(QPixmap pixmap);
    void saveImage(QString fileName);
    void setPixel(int coordX, int coordY, double value);
    Image *modificateImage(ImageModification *imgModif, Border border);
    void imageMatrixRationing();
    double getImagePixel(int coordX, int coordY, Border border);
    Image* reduceImage(Border border);
private:
    double borderCopyValue(int coordX, int coordY);
    double borderReflectionEdge(int coordX, int coordY);
    double borderTurnImage(int coordX, int coordY);
    double getAverage(int x, int y, Border border);
};

#endif // IMAGE_H
