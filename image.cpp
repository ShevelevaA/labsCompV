#include "image.h"
#include "imagemodification.h"

Image::Image(int width, int height)
{
    this->width = width;
    this->height = height;
    this->matrix = make_unique<double []>(this->width * this->height);
}

int Image::getHeight(){
    return height;
}

int Image::getWidth(){
    return width;
}

//замена изображения
void Image::changeImage(QPixmap qPixmap)
{
    QImage qImg = qPixmap.toImage();
    width = qImg.width();
    height = qImg.height();
    matrix.reset();
    matrix = make_unique<double []>(width * height);

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            QColor pixel = (QColor)qImg.pixel(x,y);
            matrix[x + y * width] = (0.213*(pixel.red()) + 0.715*((pixel.green())) + 0.072*((pixel.blue())))/255;
        }
    }
}

QPixmap Image::getImage()
{
    QImage qImg(width, height, QImage::Format_RGB32);
    QColor qColor;
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
        {
            qColor.setRgb(matrix[x + y * width]*255, matrix[x + y * width]*255, matrix[x + y * width]*255);
            qImg.setPixel(x, y, qColor.rgb());
        }

    return  QPixmap::fromImage(qImg);
}

double Image::getImagePixel(int coordX, int coordY, Border border){
    if(coordX > -1 && coordX < width && coordY > -1 && coordY < height){
        return matrix[coordX + coordY * width];
    }

    switch(border){
        case Border::Black :
        return 0;
        case Border::Copy:
        return borderCopyValue(coordX, coordY);
        case Border::Reflection:
        return borderReflectionEdge(coordX, coordY);
        default:
        return borderTurnImage(coordX, coordY); //Border::Turn
    }    
}

double Image::borderCopyValue(int coordX, int coordY){
    if(coordX < 0)
        coordX = 0;
    if(coordX >= width)
        coordX = width - 1;
    if(coordY < 0)
        coordY = 0;
    if(coordY >= height)
        coordY = height - 1;
    return matrix[coordX + coordY * width];
}

double Image::borderReflectionEdge(int coordX, int coordY){
    if(coordX < 0)
        coordX = abs(coordX);
    if(coordX >= width)
        coordX = width - (coordX - width) - 1;
    if(coordY < 0)
        coordY = abs(coordY);
    if(coordY >= height)
        coordY = height - (coordY - height) - 1;
    return matrix[coordX + coordY * width];
}

double Image::borderTurnImage(int coordX, int coordY){
    if(coordX < 0)
        coordX = width + coordX;
    if(coordX >= width)
        coordX = 1 + (coordX - width);
    if(coordY < 0)
        coordY = height + coordY;
    if(coordY >= height)
        coordY = 1 + (coordY - height);
    return matrix[coordX + coordY * width];
}

void Image::saveImage(QString fileName){
    QFile file(fileName + ".jpg");
    file.open(QIODevice::WriteOnly);
    getImage().save(&file, "JPG");
}

void Image::setPixel(int coordX, int coordY, double value){
    matrix[coordX + coordY * width] = value;
}

Image* Image::modificateImage(ImageModification *imgModif, Border border){
    Image *resultImage = new Image(this->width, this->height);

    int modHeight = imgModif->getHeight();
    int modWidth = imgModif->getWidth();

    for(int y = 0; y < this->height; y++)
        for(int x = 0; x< this->width; x++){
            double result = 0;
            for(int modY = 0; modY < modHeight; modY++)
                for(int modX = 0; modX < modWidth; modX++){
                    result += imgModif->geValuetPixelModification(modX, modY) *
                            getImagePixel(modWidth/2 + x - modX, modHeight/2 + y - modY, border);
                }
            resultImage->setPixel(x, y, result);
        }
    return resultImage;
}

Image* Image::getSobelXY(Image *imgX, Image *imgY){ //Градиент
    Image *imageXY = new Image(imgX->getWidth(), imgY->getHeight());

    for(int x = 0; x < imageXY->width; x++)
        for(int y = 0; y < imageXY->height; y++){
            imageXY->setPixel(x, y, sqrt(pow(imgX->getImagePixel(x, y, Border::Black),2) +
                                         pow(imgY->getImagePixel(x, y, Border::Black),2)));
        }
    return imageXY;
}

void Image::imageMatrixRationing(){ //Нормирование
    double max = *std::max_element(&matrix[0], &matrix[width * height]);
    double min = *std::min_element(&matrix[0], &matrix[width * height]);
    double diff = max - min;
    if(diff > 0){
        for(int x = 0; x < width; x++)
            for(int y = 0; y < height; y++){
                this->setPixel(x, y, (this->getImagePixel(x, y, Border::Black) - min)/diff);
            }
    }
}

Image* Image::reduceImage(Border border){
    int newWidth = width/2;
    int newHeight = height/2;
    Image *reduceImage = new Image(newWidth, newHeight);

    for(int x = 0; x < newWidth; x++)
        for(int y = 0; y < newHeight; y++)
        {
            reduceImage->setPixel(x, y, getAverage(x, y, border));
        }

    return reduceImage;
}

double Image::getAverage(int x, int y, Border border){
    x *= 2;
    y *= 2;
    return (getImagePixel(x, y, border) + getImagePixel(x, y + 1, border) +
            getImagePixel(x + 1, y, border) + getImagePixel(x + 1, y + 1, border))/4;
}


