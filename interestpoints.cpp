#include "interestpoints.h"

InterestPoints::InterestPoints(Image* origImage)
{
    image = origImage;
}

void InterestPoints::moravek(double thresholdValue, Border border){
    int width = image->getWidth();
    int height = image->getHeight();
    int roundPixel = 2;
    Image *imageMoravek = new Image(width, height);
    vectorPoints.clear();

    for(int x = 0; x < width; x++)
        for(int y = 0; y < height; y++){
            vector<double> errors;
            for(int dx = - 1; dx <= 1; dx++)
                for(int dy = -1; dy <= 1; dy++){
                    if(dx !=0 && dy!=0){ //сдвиг по диагонали
                        double windError = 0;
                        for(int u = -roundPixel; u <= roundPixel; u++)
                            for(int v = -roundPixel; v <= roundPixel; v++){
                                double c = pow(image->getImagePixel(x + u, y + v, border) -
                                        image->getImagePixel(x + u + dx, y + v + dy, border), 2);
                                windError += c;
                            }
                        errors.push_back(windError);
                    }
                }
            double s = *min_element(errors.begin(), errors.end());
            imageMoravek->setPixel(x, y, s);
        }
    searchPoints(imageMoravek, thresholdValue, border);
}

void InterestPoints::harris(double thresholdValue, Border border){
    int width = image->getWidth();
    int height = image->getHeight();
    int roundPixel = 2;
    Image *imageHarris = new Image(width, height);
    vectorPoints.clear();

    ImageModification *imageModifSobX = new ImageModification(Modification::SobelX);
    Image *sobelXImg = image->modificateImage(imageModifSobX, border);
    ImageModification *imageModifSobY = new ImageModification(Modification::SobelY);
    Image *sobelYImg = image->modificateImage(imageModifSobY, border);

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++){
             double A = 0;
             double B = 0;
             double C = 0;
             for(int u = -roundPixel; u <= roundPixel; u++)
                 for(int v = -roundPixel; v <= roundPixel; v++){
                     double iX = sobelXImg->getImagePixel(x + u, y + v, border);
                     double iY = sobelYImg->getImagePixel(x + u, y + v, border);
                     A += pow(iX, 2);
                     B += iX * iY;
                     C += pow(iY, 2);
                 }
             double discr = sqrt((A - C) * (A - C) + 4 * pow(B, 2));
             double l1 = abs((A + C + discr)/2);
             double l2 = abs((A + C - discr)/2);
             imageHarris->setPixel(x, y, min(l1, l2));
        }

    searchPoints(imageHarris, thresholdValue, border);
}

void InterestPoints::searchPoints(Image* imageMoravek, double t, Border border){
    int width = image->getWidth();
    int height = image->getHeight();
    int roundPixel = 2;

    for(int x = 0; x < width; x++)
        for(int y = 0; y < height; y++){
            bool localMax;
            double sXY = imageMoravek->getImagePixel(x, y, border);
            for(int u = -roundPixel; u <= roundPixel; u++)
                for(int v = -roundPixel; v <= roundPixel; v++){
                     if(u !=0 && v!=0){ //чтобы рассматривать значения s вокруг точки
                         if(sXY > imageMoravek->getImagePixel(x + u, y + v, border) && sXY > t)
                             localMax = true;
                         else{
                             localMax = false;
                             goto breakIsLocalMax;
                         }
                     }
                }
            breakIsLocalMax: if(localMax){
                Point point;
                point.x = x;
                point.y = y;
                point.s = sXY;

                vectorPoints.push_back(point);
            }
        }
}

void InterestPoints::reduceNumberOfPoints(int countPoint){
    double coef = 0.9;
    int radiusCurrent = 0;
    int radiusMax = sqrt(pow(image->getHeight(),2) + pow(image->getWidth(), 2));

    while(vectorPoints.size() > countPoint && radiusCurrent < radiusMax){
        for(int i = 0; i < vectorPoints.size(); i++)
            for(int j = i + 1; j < vectorPoints.size(); j++){ // j = i+1 тк может возникнуть казус, что размерность вектора меньше чем индекс первой точки
                if(i != j)
                if(pointAtVicinity(vectorPoints[i], vectorPoints[j], radiusCurrent) &&
                        vectorPoints[i].s > vectorPoints[j].s * coef){
                    if(vectorPoints.size() <= countPoint)
                        goto breakAll;
                    vectorPoints.erase(vectorPoints.begin() + j);
                    j--;
                }
            }
        breakAll: radiusCurrent++;
    }

}

bool InterestPoints::pointAtVicinity(Point point1, Point point2, int radius){ //радиус <= расстоянию мд точками
    int distance = sqrt(pow(point1.x - point2.x,2) + pow(point1.y - point2.y, 2));
    return distance <= radius;
}


QPixmap InterestPoints::markPointsAtImage(){
    QImage resultImage = image->getImage().toImage();
    QPainter painter(&resultImage);
    painter.setPen(qRgb(255,255,0));
    for(Point point : vectorPoints) {
        painter.drawEllipse(point.x, point.y, 2, 2);
    }
   return QPixmap::fromImage(resultImage);
}

