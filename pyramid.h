#ifndef PYRAMID_H
#define PYRAMID_H

#include "image.h"

struct Layer{
    Image *image;
    double sigmaCurrent;
    double sigmaGlobal;
};

class Pyramid
{
private:
    const double startSigma = 0.5;
    const double zeroSigma = 1.6;
    int countLayers;     //количество уровней в октаве - любое фиксированное
    int countOctaves;
    vector<vector<Layer>> vectorOctaves;
public:
    Pyramid(Image *image, int countLayersInOctave, Border border);
    int getCountLayers();
    int getCountOctaves();
    Image* getImageFromPyramid(int octaveNumber, int layerNumber);
private:
    int calculateCountOctaves(int width, int height);
    void calculatePyramidOctaves(Image *image, Border border);
    double accountDeltaSigma(double sigmaPrevious, double sigmaFollowing);
};

#endif // PYRAMID_H
