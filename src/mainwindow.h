#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QList>
#include <QLabel>
#include <QVector3D>
#include "gitldef.h"
namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void displayResult(QLabel *pcLable, QImage& cResult, QVector3D cRYB);
    void displayResult(QLabel *pcLable, QImage& cResult);
    void getBlendingResult(QImage& cImg, double dDensity);
    void generateSmoothNoise();
    double smoothNoiseInterpolation(double x, double y);
    //double turbulence(double x, double y, double size);
    void generateRandomNoise();
    QColor ryb2rgb(float inR, float inY, float inB);
private slots:
    void on_runBtn_clicked();

    void on_readFileAndRunBtn_clicked();

private:
    Ui::MainWindow *ui;

    ADD_CLASS_FIELD_PRIVATE(int,  iWidth)
    ADD_CLASS_FIELD_PRIVATE(int , iHeight)
    ADD_CLASS_FIELD_PRIVATE(QImage, cRandomNoise)
    ADD_CLASS_FIELD_PRIVATE(QImage, cSmoothed)
    ADD_CLASS_FIELD_PRIVATE(QList<QImage>, acAllWeights)
    ADD_CLASS_FIELD_PRIVATE(QImage, cResult)

    QVector3D cColor1 = QVector3D(1.0, 0.0, 0.0); //r
    QVector3D cColor2 = QVector3D(0.0, 1.0, 0.0); //y
    QVector3D cColor3 = QVector3D(0.0, 0.0, 1.0); //b
    QVector3D cColor4 = QVector3D(0.3, 0.5, 0.0); //1
    QVector3D cColor5 = QVector3D(0.5, 0.3, 0.1); //2


};

#endif // MAINWINDOW_H
