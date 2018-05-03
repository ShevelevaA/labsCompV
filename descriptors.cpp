#include "descriptors.h"
#include "interestpoints.h"

Descriptors::Descriptors(Image *image, Border inpBorder)
{
    int height = image->getHeight();
    int width = image->getWidth();
    border = inpBorder;
    ImageModification *imageModifSobX = new ImageModification(Modification::SobelX);
    Image *sobelXImg = image->modificateImage(imageModifSobX, border);

    ImageModification *imageModifSobY = new ImageModification(Modification::SobelY);
    Image *sobelYImg = image->modificateImage(imageModifSobY, border);

    lImage = Image::getSobelXY(sobelXImg, sobelYImg);
    //lImage->imageMatrixRationing();  нужно ли нормирование
    fiImage = new Image(width, height);
    for( int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            fiImage->setPixel(x, y, atan2(sobelYImg->getImagePixel(x, y, border),
                                          sobelXImg->getImagePixel(x, y, border)) * 180.0 / M_PI + 180.0);
}

Descriptor Descriptors::getDescriptorForPoint(Point point){
    Descriptor descriptor;
    descriptor.point = point;
    for(int i = 0; i < histogramsCount * binCount; i++)
        descriptor.histograms[i] = 0;

    double binSize = 360.0 / binCount;
    double radius = descriptorRadius * 2; // sigma

    for(int i = -radius; i < radius; i++)
        for(int j = -radius; j < radius; j++){

            double cosAngle = cos((360.0 - point.angle) * M_PI / 180.0);
            double sinAngle = sin((360.0 - point.angle) * M_PI / 180.0);

            //Поворачиваем
            int angledY = i * cosAngle - j * sinAngle + 0.5;
            int angledX = i * sinAngle + j * cosAngle + 0.5;

            //В пределах дескриптора?
            if(getDistanceAroundPoints((double)angledX, (double)angledY, 0.0, 0.0) <
                    sqrt(pow(radius, 2) + pow(radius, 2))){
                if(angledX < -radius)
                    angledX = 0;
                else {
                    if(angledX >= radius)
                        angledX = radius -1;
                    else
                        angledX += radius;
                }
            //angledX += radius;

                if(angledY < -radius)
                    angledY = 0;
                else {
                    if(angledY >= radius)
                        angledY = radius -1;
                    else
                        angledY += radius;
                }
            //angledY += radius;

                //Направление Фи
                double localFi = fiImage->getImagePixel(point.x + j, point.y + i, border) - point.angle;

                //За границей?
                if(localFi < 0)
                    localFi =+ 360;
                if(localFi > 360)
                    localFi =- 360;

                int x = angledX / ((radius * 2) / partsCount) - 0.5;
                int y = angledY / ((radius * 2) / partsCount) - 0.5;

                //Номер гистограммы
                int histogramNumber = x * partsCount + y;
                //Номер корзины
                int binNumber = (localFi / binSize + 0.5);

                //Раскидываем по корзинам
                double localBinCenter = (double)binNumber * binSize + binSize / 2.0;

                int binRelatedNumber;
                if(localFi < localBinCenter)
                    binRelatedNumber = binNumber - 1;
                else
                    binRelatedNumber = binNumber + 1;

                double thisCenterDistance = abs(localBinCenter - localFi);
                double relatedCenterDistance = binSize - thisCenterDistance;

                addHistValue(descriptor.histograms, histogramNumber, binNumber,
                        lImage->getImagePixel(point.x + j, point.y + i, border) * (1 - thisCenterDistance / binSize));
                addHistValue(descriptor.histograms, histogramNumber, binRelatedNumber,
                        lImage->getImagePixel(point.x + j, point.y + i, border) * (1 - relatedCenterDistance / binSize));
            }
        }
    descriptorRationing(descriptor.histograms);
    return descriptor;
}

double Descriptors::getDistanceAroundPoints(double x0, double y0, double x1, double y1){
    return sqrt(pow((x1 - x0),2) + pow((y1 - y0),2));
}

void Descriptors::addHistValue(double histograms[], int histogramNumber, int basketNumber, double value) {
    if(basketNumber >= binCount)
        basketNumber = 0;
    if(basketNumber < 0)
        basketNumber = binCount - 1;
    histograms[basketNumber * histogramsCount + histogramNumber] += value;
}


void Descriptors::descriptorRationing(double histograms[]){
    double size = 0;
    for(int i = 0; i < histogramsCount * binCount; i++)
        size += pow(histograms[i],2);
    size = sqrt(size);

    for(int i = 0; i < histogramsCount * binCount; i++){
        if(histograms[i] / size > 0.2)
            histograms[i] = 0.2;
        else
            histograms[i] = histograms[i]/size;
    }

    size = 0;
    for(int i = 0; i < histogramsCount * binCount; i++)
        size += pow(histograms[i],2);
    size = sqrt(size);

    for(int i = 0; i < histogramsCount * binCount; i++){
        histograms[i] = histograms[i] / size;
    }

}

double Descriptors::getDescriptorsDistance(Descriptor descriptor1, Descriptor descriptor2){
    double result = 0;
        for(int i = 0; i < histogramsCount; i++)
            for(int j = 0; j < binCount; j++)
                result += pow((descriptor2.histograms[i * binCount + j] - descriptor1.histograms[i * binCount + j]),2);
        return sqrt(result);
}

vector<Point> Descriptors::meth1(vector<Point> points){
    vector<Point> points1;
    const int localBinCount = 36;

    vector<double> localBin;

    for(int index = 0; index < points.size(); index++) {
      Point point = points.at(index);
       double localBinSize = 360.0 / localBinCount;
        int radius = 8;


        for(int i = 0; i < localBinCount; i++)
            if(index == 0)
                localBin.push_back(0);
            else
                localBin.at(i) = 0;

        for(int y = -radius; y < radius; y++){
            for(int x = -radius; x < radius; x++){

                //В пределах ?
                if(getDistanceAroundPoints((double)x, (double)y, 0.0, 0.0) < sqrt(pow(radius,2) + pow(radius,2))){

                    double localPfi =  fiImage->getImagePixel(point.x + x, point.y + y, border);
                    int binNumber = (localPfi / localBinSize + 0.5);

                    if(binNumber >= localBinCount)
                        binNumber = 0;
                    else
                        if(binNumber < 0)
                            binNumber = localBinCount - 1;

                    double localBinCenter = (double)binNumber * localBinSize + localBinSize / 2.0;

                    int relatedBin;
                    if(localPfi < localBinCenter)
                        relatedBin = binNumber - 1;
                    else
                        relatedBin = binNumber + 1;

                    double thisCenterDistance = abs(localBinCenter - localPfi);
                    double relatedCenterDistance = localBinSize - thisCenterDistance;

                    //отдать в метод
                    if(relatedBin >= localBinCount)
                        relatedBin = 0;
                    else
                        if(relatedBin < 0)
                            relatedBin = localBinCount -1;
                    //

                    localBin.at(binNumber) += lImage->getImagePixel(point.x + x, point.y + y, border) * (1 - thisCenterDistance / localBinSize);
                    localBin.at(relatedBin) += lImage->getImagePixel(point.x + x, point.y + y, border) * (1 - relatedCenterDistance / localBinSize);
                }
            }
        }

        double firstMaxValue = -1;
        int firstMaxValueIndex = -1;
        double secondMaxValue = -1;
        int secondMaxValueIndex = -1;

        //Ищем первую и вторую максимальную
        for(int i = 0; i < localBinCount; i++){
            if(localBin.at(i) > firstMaxValue){
                secondMaxValue = firstMaxValue;
                secondMaxValueIndex = firstMaxValueIndex;

                firstMaxValue = localBin.at(i);
                firstMaxValueIndex = i;
            } else {
                if(localBin.at(i) > secondMaxValue){
                    secondMaxValue = localBin.at(i);
                    secondMaxValueIndex = i;
                }
            }
        }

        //добавили первую
        Point firstPoint = point;
        firstPoint.angle = firstMaxValueIndex * localBinSize;
        points1.push_back(firstPoint);

        //если вторая >= 0.8 от первой, то добваляем то же
        if(secondMaxValue >= (firstMaxValue * 0.8)){
            Point secondPoint = point;
            secondPoint.angle = secondMaxValueIndex * localBinSize;
            points1.push_back(secondPoint);
        }
    }
    return points1;
}
