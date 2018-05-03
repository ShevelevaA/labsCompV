#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "helper.h"
#include "image.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_LoadOrig_clicked();

    void on_pushButton_Grey_clicked();

    void on_pushButton_SaveImg_clicked();

    void on_pushButton_Sobel_clicked();

    void on_pushButton_Gaus_clicked();

    void on_pushButton_PyramidShow_clicked();

    void on_pushButton_Moravek_clicked();

    void on_pushButton_Harris_clicked();

    void on_pushButton_Descriptor_clicked();

    void on_pushButton_Descriptor_2_clicked();

private:
    void showImageAtSecondScreen(QPixmap pixmap);
    Ui::MainWindow *ui;
    QString filePath;
    Helper *displayHelper;
    Image *image;
};

#endif // MAINWINDOW_H
