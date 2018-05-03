#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include "image.h"
#include "interestpoints.h"

const int binCount = 8;
const int histogramsCount = 16;

struct Descriptor{
    Point point;
    double histograms[histogramsCount * binCount];
};

class Descriptors
{
private:
    Image *lImage;
    Image *fiImage;
    Border border;
    const int partsCount = 4;
    const double descriptorRadius = 8.0;
public:
    Descriptors(Image *image, Border inpBorder);
    Descriptor getDescriptorForPoint(Point point);
    static double getDescriptorsDistance(Descriptor descriptor1, Descriptor descriptor2);
    std::vector<Point> orientPoints(std::vector<Point> points);
vector<Point> meth1(vector<Point> points);
private:
    double getDistanceAroundPoints(double x0, double y0, double x1, double y1);
    double getHistValue(const double histograms[], int histogramNumber, int basketNumber);
    void Descriptors::addHistValue(double histograms[], int histogramNumber, int basketNumber, double value);
    void Descriptors::setHistValue(double histograms[], int histogramNumber, int basketNumber, double value);
    void descriptorRationing(double histograms[]);
};

#endif // DESCRIPTORS_H
