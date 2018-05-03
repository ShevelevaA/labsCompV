#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "helper.cpp"
#include "image.cpp"
#include "imagemodification.cpp"
#include "pyramid.cpp"
#include "interestpoints.cpp"
#include "descriptors.cpp"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    filePath = "C:\\Users\\ks\\Desktop\\ComVPic\\";
    displayHelper = new Helper();
    image = new Image(0,0);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete displayHelper;
}

void MainWindow::showImageAtSecondScreen(QPixmap qPixmap){
    QGraphicsScene *scene = new QGraphicsScene();
    scene->addPixmap(qPixmap);
    ui->graphicsView_Edit->setScene(scene);
    ui->graphicsView_Edit->show();
}

void MainWindow::on_pushButton_LoadOrig_clicked()
{
    QPixmap qPixmap = displayHelper->choiseImg();
    if(!qPixmap.isNull()){
        QGraphicsScene *scene = new QGraphicsScene();
        scene->addPixmap(qPixmap);
        image->changeImage(qPixmap);
        ui->graphicsView_Orig->setScene(scene);
        ui->graphicsView_Orig->show();
    }
    else{
        ui->label_Messages->setText("Load error!");
    }
}

void MainWindow::on_pushButton_Grey_clicked()
{
    showImageAtSecondScreen(image->getImage());

    image->saveImage(filePath + "greyImg");
}

void MainWindow::on_pushButton_SaveImg_clicked()
{

}

void MainWindow::on_pushButton_Sobel_clicked()
{
    //Собель
    ImageModification *imageModifSobX = new ImageModification(Modification::SobelX);
    Image *sobelXImg = image->modificateImage(imageModifSobX, Border::Black);

    ImageModification *imageModifSobY = new ImageModification(Modification::SobelY);
    Image *sobelYImg = image->modificateImage(imageModifSobY, Border::Black);

    Image *sobelXYImg = Image::getSobelXY(sobelXImg, sobelYImg);
    sobelXYImg->imageMatrixRationing();
    showImageAtSecondScreen(sobelXYImg->getImage());
    sobelXYImg->saveImage(filePath + "sobelXYImg");
    //Сохранение Собеля по x и по y
    sobelXImg->imageMatrixRationing();
    sobelXImg->saveImage(filePath + "sobelXImg");
    sobelYImg->imageMatrixRationing();
    sobelYImg->saveImage(filePath + "sobelYImg");
}

void MainWindow::on_pushButton_Gaus_clicked()
{
    //Гаусс
    ImageModification *imageModifGaussX = new ImageModification(Modification::GaussX, 6);
    Image *gaussXImg = image->modificateImage(imageModifGaussX, Border::Black);

    ImageModification *imageModifGaussY = new ImageModification(Modification::GaussY, 6);
    Image *gaussXYImg = gaussXImg->modificateImage(imageModifGaussY, Border::Black);
    showImageAtSecondScreen(gaussXYImg->getImage());

    gaussXYImg->saveImage(filePath + "gaussXYImg");
}

void MainWindow::on_pushButton_PyramidShow_clicked()
{
    Pyramid *pyramid = new Pyramid(image, 4, Border::Black);
    int countOctaves = pyramid->getCountOctaves();
    int countLayers = pyramid->getCountLayers();

    for(int i = 0; i < countOctaves; i++)
        for(int j = 0; j < countLayers; j++){
            Image *imagePyramid = pyramid->getImageFromPyramid(i, j);
            imagePyramid->saveImage(filePath + "pyramidImg" + QString::number(i) + QString::number(j));
        }

    showImageAtSecondScreen(pyramid->getImageFromPyramid(ui->spinBox_Octave->value(), ui->spinBox_Layer->value())->getImage());
}

void MainWindow::on_pushButton_Moravek_clicked()
{
    InterestPoints *moravekPoints = new InterestPoints(image);
    moravekPoints->moravek(0.1, Border::Reflection);
    moravekPoints->reduceNumberOfPoints(500);
    QPixmap moravekQPixmap = moravekPoints->markPointsAtImage();
    showImageAtSecondScreen(moravekQPixmap);
    displayHelper->saveQPixmap(moravekQPixmap, filePath + "moravekImg");
}

void MainWindow::on_pushButton_Harris_clicked()
{
    InterestPoints *harrisPoints = new InterestPoints(image);
    harrisPoints->harris(0.3, Border::Reflection);
    harrisPoints->reduceNumberOfPoints(100);
    QPixmap harrisQPixmap = harrisPoints->markPointsAtImage();
    showImageAtSecondScreen(harrisQPixmap);
    displayHelper->saveQPixmap(harrisQPixmap, filePath + "harrisImg");
}



void MainWindow::on_pushButton_Descriptor_clicked()
{
    Border border = Border::Reflection;
    int pointsCount = 100;
    /*
     *
     *
     * FIXME harrisPoints1->getPoints() на переменную типа vector<Point>
     *
     *
     */
    Image *image1 = displayHelper->loadImg();
    Descriptors *descriptors1 = new Descriptors(image1, border);
    vector<Descriptor> vectorDescriptors1;
    InterestPoints *harrisPoints1 = new InterestPoints(image1);
    harrisPoints1->harris(0.3, border);
    harrisPoints1->reduceNumberOfPoints(pointsCount);

    for(int i = 0; i < harrisPoints1->getPoints().size(); i++){
        vectorDescriptors1.push_back(descriptors1->getDescriptorForPoint(harrisPoints1->getPoints().at(i)));
    }

    Image *image2 = displayHelper->loadImg();
    Descriptors *descriptors2 = new Descriptors(image2, border);
    vector<Descriptor> vectorDescriptors2;
    InterestPoints *harrisPoints2 = new InterestPoints(image2);
    harrisPoints2->harris(0.3, border);
    harrisPoints2->reduceNumberOfPoints(pointsCount);

    for(int i = 0; i < harrisPoints2->getPoints().size(); i++){
        vectorDescriptors2.push_back(descriptors2->getDescriptorForPoint(harrisPoints2->getPoints().at(i)));
    }

    QImage qImage1 = image1->getImage().toImage();
    QImage qImage2 = image2->getImage().toImage();
    int height;
    if(qImage1.height() > qImage2.height())
        height = qImage1.height();
    else
        height = qImage2.height();

    QImage qImage( qImage1.width() + qImage2.width(), height, qImage1.format());

    QPainter painter;
    painter.begin(&qImage);
    painter.drawImage(QPoint(0,0), qImage1);
    painter.drawImage(QPoint(qImage1.width(),0), qImage2);
    QColor color = QColor(qRgb(0,255,0));

    for(int i = 0; i < harrisPoints1->getPoints().size(); i++){
        Point point = harrisPoints1->getPoints().at(i);
        painter.setPen(color);
        painter.drawRect(point.x - 5, point.y - 5, 10, 10);
    }

    for(int i = 0; i < harrisPoints2->getPoints().size(); i++){
        Point point = harrisPoints2->getPoints().at(i);
        painter.setPen(color);
        painter.drawRect(point.x - 5 + qImage1.width(), point.y - 5, 10, 10);
    }

    for(int i = 0; i < vectorDescriptors1.size(); i++){
        double firstMinValue = 10000;
        int firstMinValueIndex = 10000;
        double secondMinValue = 10000;
        int secondMinValueIndex = 10000;

        for(int j = 0; j < vectorDescriptors2.size(); j++){
            double dist = Descriptors::getDescriptorsDistance(vectorDescriptors1.at(i), vectorDescriptors2.at(j));
            if(dist < firstMinValue){
                secondMinValue = firstMinValue;
                secondMinValueIndex = firstMinValueIndex;

                firstMinValue = dist;
                firstMinValueIndex = j;
            } else {
                if(dist < secondMinValue){
                    secondMinValue = dist;
                    secondMinValueIndex = j;
                }
            }
        }

        if(firstMinValue / secondMinValue < 0.8){
            QPen pen(QColor(255, 0, 0));
            painter.setPen(pen);
            painter.drawLine(QPoint(vectorDescriptors1.at(i).point.x, vectorDescriptors1.at(i).point.y),
                             QPoint(vectorDescriptors2.at(firstMinValueIndex).point.x + qImage1.width(), vectorDescriptors2.at(firstMinValueIndex).point.y));
        }
    }

    painter.end();

    showImageAtSecondScreen(QPixmap::fromImage(qImage));
    displayHelper->saveQPixmap(QPixmap::fromImage(qImage), filePath + "Descriptor");
}


void MainWindow::on_pushButton_Descriptor_2_clicked()
{

    Border border = Border::Reflection;
    int pointsCount = 100;

    Image *image1 = displayHelper->loadImg();
    Descriptors *descriptors1 = new Descriptors(image1, border);
    vector<Descriptor> vectorDescriptors1;
    InterestPoints *harrisPoints1 = new InterestPoints(image1);
    harrisPoints1->harris(0.3, border);
    harrisPoints1->reduceNumberOfPoints(pointsCount);
    //vector<Point> points1 = descriptors1->orientPoints(harrisPoints1->getPoints());
    vector<Point> points1 = descriptors1->meth1(harrisPoints1->getPoints());

    for(int i = 0; i < points1.size(); i++){
        vectorDescriptors1.push_back(descriptors1->getDescriptorForPoint(points1.at(i)));
    }

    Image *image2 = displayHelper->loadImg();
    Descriptors *descriptors2 = new Descriptors(image2, border);
    vector<Descriptor> vectorDescriptors2;
    InterestPoints *harrisPoints2 = new InterestPoints(image2);
    harrisPoints2->harris(0.3, border);
    harrisPoints2->reduceNumberOfPoints(pointsCount);
    vector<Point> points2 = descriptors2->meth1(harrisPoints2->getPoints());

    for(int i = 0; i < points2.size(); i++){
        vectorDescriptors2.push_back(descriptors2->getDescriptorForPoint(points2.at(i)));
    }

    QImage qImage1 = image1->getImage().toImage();
    QImage qImage2 = image2->getImage().toImage();
    int height;
    if(qImage1.height() > qImage2.height())
        height = qImage1.height();
    else
        height = qImage2.height();

    QImage qImage( qImage1.width() + qImage2.width(), height, qImage1.format());

    QPainter painter;
    painter.begin(&qImage);
    QColor color = QColor(qRgb(0,255,0));

    painter.drawImage(QPoint(0,0), qImage1);
    painter.drawImage(QPoint(qImage1.width(),0), qImage2);

    for(int i = 0; i < points1.size(); i++){
        painter.setPen(color);
        painter.drawEllipse(points1.at(i).x - 8, points1.at(i).y - 8, 16, 16);
        painter.drawLine(
                    points1.at(i).x,
                    points1.at(i).y,
                    points1.at(i).x + (15 * cos(points1.at(i).angle * M_PI / 180.0)),
                    points1.at(i).y + (15 * sin(points1.at(i).angle * M_PI / 180.0)) );
    }

    for(int i = 0; i < points2.size(); i++){
        painter.setPen(color);
        painter.drawEllipse(points2.at(i).x - 8 + qImage1.width(), points2.at(i).y - 8, 16, 16);
        painter.drawLine(
                    points2.at(i).x + qImage1.width(),
                    points2.at(i).y,
                    points2.at(i).x + (15 * cos(points2.at(i).angle * M_PI / 180.0)) + qImage1.width(),
                    points2.at(i).y + (15 * sin(points2.at(i).angle * M_PI / 180.0)) );
    }

    //Поиск
    int findCount = 0;
    for(int i = 0; i < vectorDescriptors1.size(); i++){
        double firstMinValue = 10000;
        int firstMinValueIndex = 10000;
        double secondMinValue = 10000;
        int secondMinValueIndex = 10000;

        for(int j = 0; j < vectorDescriptors2.size(); j++){
            double dist = Descriptors::getDescriptorsDistance(vectorDescriptors1.at(i), vectorDescriptors2.at(j));
            if(dist < firstMinValue){
                secondMinValue = firstMinValue;
                secondMinValueIndex = firstMinValueIndex;

                firstMinValue = dist;
                firstMinValueIndex = j;
            } else {
                if(dist < secondMinValue){
                    secondMinValue = dist;
                    secondMinValueIndex = j;
                }
            }
        }

        if(firstMinValue / secondMinValue < 0.8){
            findCount++;
            QPen pen(QColor(rand()%255 ,rand()%255, rand()%255));
            painter.setPen(pen);

            painter.drawEllipse(points1.at(i).x - 8, points1.at(i).y - 8, 16, 16);
            painter.drawLine(
                        points1.at(i).x,
                        points1.at(i).y,
                        points1.at(i).x + (15 * cos(points1.at(i).angle * M_PI / 180.0)),
                        points1.at(i).y + (15 * sin(points1.at(i).angle * M_PI / 180.0)) );

            painter.drawEllipse(points2.at(firstMinValueIndex).x - 8 + qImage1.width(), points2.at(firstMinValueIndex).y - 8, 16, 16);
            painter.drawLine(
                        points2.at(firstMinValueIndex).x + qImage1.width(),
                        points2.at(firstMinValueIndex).y,
                        points2.at(firstMinValueIndex).x + (15 * cos(points2.at(firstMinValueIndex).angle * M_PI / 180.0)) + qImage1.width(),
                        points2.at(firstMinValueIndex).y + (15 * sin(points2.at(firstMinValueIndex).angle * M_PI / 180.0)) );

            painter.drawLine(
                        QPoint(vectorDescriptors1.at(i).point.x,
                               vectorDescriptors1.at(i).point.y),
                        QPoint(vectorDescriptors2.at(firstMinValueIndex).point.x + qImage1.width(),
                               vectorDescriptors2.at(firstMinValueIndex).point.y));
        }
    }

    painter.end();

    showImageAtSecondScreen(QPixmap::fromImage(qImage));
    displayHelper->saveQPixmap(QPixmap::fromImage(qImage), filePath + "Descriptor2");

}
