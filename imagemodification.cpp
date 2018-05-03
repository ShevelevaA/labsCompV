#include "imagemodification.h"

ImageModification::ImageModification(Modification modification)
{
    switch(modification){
    case Modification::SobelX:
        height = 3;
        width = 3;
        feelMatrixSobelX();
        break;
    case Modification::SobelY:
        height = 3;
        width = 3;
        feelMatrixSobelY();
        break;
    default: ;
    }
}

ImageModification::ImageModification(Modification modification, double sigma)
{
    switch(modification){
    case Modification::GaussX:
        height = 1;
        width = getDimentionRadius(sigma);
        feelGaussOneCoord(sigma);
        break;
    case Modification::GaussY:
        height = getDimentionRadius(sigma);
        width = 1;
        feelGaussOneCoord(sigma);
        break;
    default: ;
    }
}

void ImageModification::feelMatrixSobelX(){ // { -1, 0, 1, -2, 0, 2, -1, 0, 1 }
    matrixModification =  make_unique<double []>(height * width);
    matrixModification[0] = -1;
    matrixModification[1] = 0;
    matrixModification[2] = 1;
    matrixModification[3] = -2;
    matrixModification[4] = 0;
    matrixModification[5] = 2;
    matrixModification[6] = -1;
    matrixModification[7] = 0;
    matrixModification[8] = 1;
}

void ImageModification::feelMatrixSobelY(){ // { -1, -2, -1, 0, 0, 0, 1, 2, 1 }
    matrixModification =  make_unique<double []>(height * width);
    matrixModification[0] = -1;
    matrixModification[1] = -2;
    matrixModification[2] = -1;
    matrixModification[3] = 0;
    matrixModification[4] = 0;
    matrixModification[5] = 0;
    matrixModification[6] = 1;
    matrixModification[7] = 2;
    matrixModification[8] = 1;
}

int ImageModification::getHeight(){
    return height;
}

int ImageModification::getWidth(){
    return width;
}

double ImageModification::geValuetPixelModification(int coordX, int coordY){
    return matrixModification[coordX + coordY * width];
}

int ImageModification::getDimentionRadius(double sigma){
    int first = (int)(sigma * 3);
    return first * 2;
}

void ImageModification::feelGaussOneCoord(double sigma){
    int dimention = getDimentionRadius(sigma);
    double  multiplier = 1.0/(sqrt(2 * M_PI) * sigma);
    matrixModification = make_unique<double []>(dimention);

    double sum = 0;
    for(int i = 0; i < dimention; i++){
        int coord = i - dimention/2;
        double expon = pow(M_E, ((-1.0) * pow(coord, 2))/(2 * pow(sigma, 2)));
        matrixModification[i] = multiplier * expon;
        sum += matrixModification[i];
    }

    for(int i = 0; i < dimention; i++){
        matrixModification[i]/=sum;
    }
}
