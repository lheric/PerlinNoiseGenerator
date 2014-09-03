#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QList>
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
    void generateNoiseImage();

    double smoothNoise(double x, double y);
    double turbulence(double x, double y, double size);
    void generateNoise();
private slots:
    void on_runBtn_clicked();

private:
    Ui::MainWindow *ui;

    ADD_CLASS_FIELD_PRIVATE(int,  iWidth)
    ADD_CLASS_FIELD_PRIVATE(int , iHeight)
    ADD_CLASS_FIELD_PRIVATE(QImage, cRandomNoise)
    ADD_CLASS_FIELD_PRIVATE(QImage, cSmoothed)
    ADD_CLASS_FIELD_PRIVATE(QList<QImage>, acAllSize)
    ADD_CLASS_FIELD_PRIVATE(QImage, cResult)

};

#endif // MAINWINDOW_H