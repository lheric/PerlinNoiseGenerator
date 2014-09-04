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
    m_iHeight = 256;
    m_iWidth = 256;
    m_cResult = QImage(m_iWidth, m_iHeight, QImage::Format_ARGB32);
    m_cRandomNoise = QImage(m_iWidth, m_iHeight, QImage::Format_RGB32);
    m_cSmoothed = QImage(m_iWidth, m_iHeight, QImage::Format_RGB32);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateRandomNoise()
{
    for (int x = 0; x < m_iWidth; x++)
    {
        for (int y = 0; y < m_iHeight; y++)
        {
            int iRand = qrand() % 255;
            m_cRandomNoise.setPixel(x,y, qRgb(iRand, iRand, iRand));
        }
    }




}

double MainWindow::smoothNoiseInterpolation(double x, double y)
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

//double MainWindow::turbulence(double x, double y, double size)
//{
//    double value = 0.0, initialSize = size;

//    while(size >= 1)
//    {
//        value += smoothNoiseInterpolation(x / size, y / size) * size;
//        size /= 2.0;
//    }

//    return(128.0 * value / initialSize);
//}
void MainWindow::displayResult(QLabel* pcLable, QImage& cResult)
{
    pcLable->setPixmap(QPixmap::fromImage(cResult));
    cResult.save(QString("Perlin_Noise_%1x%2.png").arg(m_iWidth).arg(m_iHeight));
}

void MainWindow::getBlendingResult(QImage& cImg)
{
    for (int x = 0; x < m_iWidth; x++)
    {
        for (int y = 0; y < m_iHeight; y++)
        {
            double size = 8.0;
            double value = 0.0;
            double initialSize = size;

            while(size >= 1)
            {
                value += smoothNoiseInterpolation(x / size, y / size) * size;
                size /= 2.0;
            }

            value = value / (initialSize*2);
            double percent = value/255;
            double oneMinusPercent = 1-percent;
            cImg.setPixel(x, y, qRgb(255*percent,255*percent,255*percent));
        }
    }
}

void MainWindow::generateSmoothNoise()
{
    for (int x = 0; x < m_iWidth; x++)
    {
        for (int y = 0; y < m_iHeight; y++)
        {

            int iGray = smoothNoiseInterpolation(x/8.0,y/8.0);
            m_cSmoothed.setPixel(x,y, qRgb(iGray,iGray,iGray));
        }
    }
}

void MainWindow::on_runBtn_clicked()
{
    QImage cPerlinNoise(m_iWidth, m_iHeight, QImage::Format_RGB32);
    m_acAllWeights.clear();
    // first
    this->generateRandomNoise();
    this->generateSmoothNoise();
    this->getBlendingResult(cPerlinNoise);
    this->displayResult(ui->random1, cPerlinNoise);
    m_acAllWeights.push_back(cPerlinNoise);

    // second
    this->generateRandomNoise();
    this->generateSmoothNoise();
    this->getBlendingResult(cPerlinNoise);
    this->displayResult(ui->random2, cPerlinNoise);
    m_acAllWeights.push_back(cPerlinNoise);

    // result
    QColor cColor1 = QColor(255, 0, 0);
    QColor cColor2 = QColor(0, 255, 0);
    QColor cBlending;
    for(int x = 0; x < m_iWidth; x++)
    {
        for(int y = 0; y < m_iHeight; y++)
        {
            double dWeight1 = qGray(m_acAllWeights[0].pixel(x,y))/255.0;
            double dWeight2 = qGray(m_acAllWeights[1].pixel(x,y))/255.0;
            int r = cColor1.red()*dWeight1 + cColor2.red()*dWeight2;
            cBlending.setRed(VALUE_CLIP(0, 255, r));
            int g = cColor1.green()*dWeight1 + cColor2.green()*dWeight2;
            cBlending.setGreen(VALUE_CLIP(0, 255, g));
            int b = cColor1.blue()*dWeight1 + cColor2.blue()*dWeight2;
            cBlending.setBlue(VALUE_CLIP(0, 255, b));


            //qDebug() << dWeight1 << dWeight2;
            m_cResult.setPixel(x, y, cBlending.rgba());
        }
    }
    this->displayResult(ui->resultLabel, m_cResult);

}
