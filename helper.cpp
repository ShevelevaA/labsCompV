#include "helper.h"
#include <QFileDialog>

Helper::Helper()
{

}

QPixmap Helper::choiseImg()
{
    QString imgName = QFileDialog::getOpenFileName(0, "Open dialog", "", "*.jpg *.png");
    if (!imgName.isEmpty())
    {
        QPixmap qPixmap;
        if(!qPixmap.load(imgName)){
            return NULL;
        }
        return qPixmap;
    }
    return NULL;
}

void Helper::saveQPixmap(QPixmap image, QString fileName){
    QFile file(fileName + ".jpg");
    file.open(QIODevice::WriteOnly);
    image.save(&file, "JPG");
}

Image* Helper::loadImg(){
    Image* image = new Image(0,0);

    QPixmap qPixmap = choiseImg();
    if(!qPixmap.isNull()){
        image->changeImage(qPixmap);
    }
    return image;
}
