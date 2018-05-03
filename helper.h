#ifndef DISPLAYHELPER_H
#define DISPLAYHELPER_H

#include "image.h"

#include <QPixmap>
#include <QGraphicsScene>

class Helper
{
public:
    Helper();
    QPixmap choiseImg();
    void saveQPixmap(QPixmap image, QString fileName);
    Image* loadImg();
};

#endif // DISPLAYHELPER_H
