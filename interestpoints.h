#ifndef INTERESTPOINTS_H
#define INTERESTPOINTS_H

#include "image.h"


struct Point{
    int x;
    int y;
    double s;
    double angle = 0.0;
};

class InterestPoints
{
private:
    Image *image;
public:
    vector<Point> vectorPoints;
public:
    InterestPoints(Image *origImage);
    void moravek(double thresholdValue, Border border);
    void harris(double thresholdValue, Border border);
    void reduceNumberOfPoints(int countPoint);
    QPixmap markPointsAtImage();  //нанести точки интереса на изображение
    vector<Point> getPoints(){return vectorPoints;}
private:
    void searchPoints(Image *imageMoravek, double t, Border border);
    bool pointAtVicinity(Point point1, Point point2, int radius);
};

#endif // INTERESTPOINTS_H
