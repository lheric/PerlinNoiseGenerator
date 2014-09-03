#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTime>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qsrand(QTime::currentTime().msec());
    m_iHeight = 128;
    m_iWidth = 128;
    m_cResult = QImage(m_iWidth, m_iHeight, QImage::Format_RGB32);
    m_cRandomNoise = QImage(m_iWidth, m_iHeight, QImage::Format_RGB32);
    m_cSmoothed = QImage(m_iWidth, m_iHeight, QImage::Format_RGB32);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateNoise()
{
    for (int x = 0; x < m_iWidth; x++)
    {
        for (int y = 0; y < m_iHeight; y++)
        {
            int iRand = qrand() % 255;
            m_cRandomNoise.setPixel(x,y, qRgb(iRand, iRand, iRand));
            int iGray = smoothNoise(x/8.0,y/8.0);
            m_cSmoothed.setPixel(x,y, qRgb(iGray,iGray,iGray));
        }
    }



    for (int x = 0; x < m_iWidth; x++)
    {
        for (int y = 0; y < m_iHeight; y++)
        {
            double size = 32.0;
            double value = 0.0;
            double initialSize = size;

            while(size >= 1)
            {
                value += smoothNoise(x / size, y / size) * size;
                size /= 2.0;
            }

            value = value / (initialSize*2);
            m_cResult.setPixel(x, y, qRgb(value,value,value));
        }
    }
}

double MainWindow::smoothNoise(double x, double y)
{
   //get fractional part of x and y
   double fractX = x - int(x);
   double fractY = y - int(y);

   //wrap around
   int x1 = (int(x) + m_iWidth)  % m_iWidth;
   int y1 = (int(y) + m_iHeight) % m_iHeight;

   //neighbor values
   int x2 = (x1 + m_iWidth - 1) % m_iWidth;
   int y2 = (y1 + m_iHeight - 1) % m_iHeight;

   //smooth the noise with bilinear interpolation
   double value = 0.0;
   value += fractX       * fractY       * qGray(m_cRandomNoise.pixel(x1,y1));
   value += fractX       * (1 - fractY) * qGray(m_cRandomNoise.pixel(x1,y2));
   value += (1 - fractX) * fractY       * qGray(m_cRandomNoise.pixel(x2,y1));
   value += (1 - fractX) * (1 - fractY) * qGray(m_cRandomNoise.pixel(x2,y2));

   return value;

}

double MainWindow::turbulence(double x, double y, double size)
{
    double value = 0.0, initialSize = size;

    while(size >= 1)
    {
        value += smoothNoise(x / size, y / size) * size;
        size /= 2.0;
    }

    return(128.0 * value / initialSize);
}
void MainWindow::generateNoiseImage()
{
//    int iSize = 100;
//    double** ppdGradient = new double*[iSize];
//    for(int i = 0; i < iSize; i++)
//    {
//        ppdGradient[i] = new double[iSize];
//        for(int j = 0; j < iSize; j++)
//            ppdGradient[i][j] = 0;
//    }

//    m_cResult = QPixmap(iSize, iSize);
//    m_cResult.fill(Qt::red);

//    qsrand(QTime::currentTime().msec());
//    qDebug() << qrand()%6;



    generateNoise();

    ui->resultLabel->setPixmap(QPixmap::fromImage(m_cResult));
}

void MainWindow::on_runBtn_clicked()
{
    this->generateNoiseImage();
}
