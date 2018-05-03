#include "pyramid.h"

Pyramid::Pyramid(Image *image, int countLayersInOctave, Border border)
{
    countLayers = countLayersInOctave;
    countOctaves = calculateCountOctaves(image->getWidth(), image->getHeight());
    calculatePyramidOctaves(image, border);
}

int Pyramid::getCountLayers(){
    return countLayers;
}

int Pyramid::getCountOctaves(){
    return countOctaves;
}

int Pyramid::calculateCountOctaves(int width, int height){
    int minSide;

    if(width < height)
        minSide = width;
    else
        minSide = height;

    return log(minSide);
}

void Pyramid::calculatePyramidOctaves(Image *image, Border border){
    double k = pow(2, 1.0/countLayers);

    for(int i = 0; i < countOctaves; i++){
        vector<Layer> layerVector;
        if(i == 0){
            Layer layer;
            layer.sigmaCurrent = zeroSigma;
            layer.sigmaGlobal = zeroSigma;
            double deltaSigma = accountDeltaSigma(startSigma, layer.sigmaCurrent);

            ImageModification *imageModifGaussX = new ImageModification(Modification::GaussX, deltaSigma);
            Image *gaussXImg = image->modificateImage(imageModifGaussX, border);
            ImageModification *imageModifGaussY = new ImageModification(Modification::GaussY, deltaSigma);
            layer.image = gaussXImg->modificateImage(imageModifGaussY, border);

            layerVector.push_back(layer);
        }
        else{
            vector<Layer> octavePrevious = vectorOctaves[i - 1];

            Layer layer;
            layer.sigmaCurrent = octavePrevious[countLayers - 1].sigmaCurrent / 2;
            layer.sigmaGlobal = octavePrevious[countLayers - 1].sigmaGlobal * 2;

            layer.image = (octavePrevious[countLayers - 1].image)->reduceImage(border);
            layerVector.push_back(layer);
        }
        for(int j = 1; j < countLayers; j++){
            Layer layer;
            layer.sigmaCurrent = layerVector[j - 1].sigmaCurrent * k;
            layer.sigmaGlobal = layerVector[j - 1].sigmaGlobal * k;
            double deltaSigma = accountDeltaSigma(layerVector[j - 1].sigmaCurrent, layer.sigmaCurrent);

            ImageModification *imageModifGaussX = new ImageModification(Modification::GaussX, deltaSigma);
            Image *gaussXImg = layerVector[j - 1].image->modificateImage(imageModifGaussX, border);
            ImageModification *imageModifGaussY = new ImageModification(Modification::GaussY, deltaSigma);
            layer.image = gaussXImg->modificateImage(imageModifGaussY, border);

            layerVector.push_back(layer);
        }
        vectorOctaves.push_back(layerVector);
    }
}

double Pyramid::accountDeltaSigma(double sigmaPrevious, double sigmaFollowing){
    return sqrt(pow(sigmaFollowing, 2) - pow(sigmaPrevious, 2));
}

Image* Pyramid::getImageFromPyramid(int octaveNumber, int layerNumber){
    if(octaveNumber >= countOctaves)
        octaveNumber = countOctaves - 1;
    if(octaveNumber < 0)
        octaveNumber = 0;

    if(layerNumber >= countLayers)
        layerNumber = countLayers - 1;
    if(layerNumber < 0)
        layerNumber = 0;

    return vectorOctaves[octaveNumber][layerNumber].image;
}
