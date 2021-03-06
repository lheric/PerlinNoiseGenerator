#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTime>
#include <QVector3D>
#include <QFile>
#include <QStringList>
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


    cColor1 = QVector3D(1.0, 0.0, 0.0); //r
    cColor2 = QVector3D(0.0, 1.0, 0.0); //y
    cColor3 = QVector3D(0.0, 0.0, 1.0); //b
    cColor4 = QVector3D(0.3, 0.5, 0.0); //1
    cColor5 = QVector3D(0.5, 0.3, 0.1); //2
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

QColor MainWindow::ryb2rgb(float inR, float inY, float inB)
{

    const static QVector3D f000(1.0,     1.0,        1.0);
    const static QVector3D f001(0.163,   0.373,      0.6);
    const static QVector3D f010(1.0,     1.0,        0.0);
    const static QVector3D f011(0.0,     0.66,       0.2);
    const static QVector3D f100(1.0,     0.0,        0.0);
    const static QVector3D f101(0.5,     0.5,        0.0);
    const static QVector3D f110(1.0,     0.5,        0.0);
    const static QVector3D f111(0.2,     0.094,      0.0);
    float oneMinusR = 1-inR;
    float oneMinusY = 1-inY;
    float oneMinusB = 1-inB;
    static QVector3D rgb;
    rgb = f000*oneMinusR*oneMinusY*oneMinusB + f001*oneMinusR*oneMinusY*inB
            + f010*oneMinusR*inY*oneMinusB + f100*inR*oneMinusY*oneMinusB +
            f011*oneMinusR*inY*inB + f101*inR*oneMinusY*inB +
            f110*inR*inY*oneMinusB + f111*inR*inY*inB;

    rgb *= 255;

    float outR = qMax(0, qMin(int(rgb.x()), 255));
    float outG = qMax(0, qMin(int(rgb.y()), 255));
    float outB = qMax(0, qMin(int(rgb.z()), 255));
    return QColor(outR, outG, outB);
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
void MainWindow::displayResult(QLabel* pcLable, QImage& cResult, QVector3D cRYB)
{
    QImage cColorImage(cResult);
    QVector3D cWeightColor;
    for(int x = 0; x < cResult.width(); x++)
    {
        for(int y = 0; y < cResult.height(); y++)
        {
            cWeightColor = cRYB * qGray(cResult.pixel(x,y))/255.0;
            QColor rgb = ryb2rgb(cWeightColor.x(), cWeightColor.y(), cWeightColor.z());
            cColorImage.setPixel(x,y, rgb.rgb());
        }
    }
    pcLable->setPixmap(QPixmap::fromImage(cColorImage));

    //cResult.save(QString("Perlin_Noise_%1x%2.png").arg(m_iWidth).arg(m_iHeight));
}

void MainWindow::displayResult(QLabel* pcLable, QImage& cResult)
{
    pcLable->setPixmap(QPixmap::fromImage(cResult));

    //cResult.save(QString("Perlin_Noise_%1x%2.png").arg(m_iWidth).arg(m_iHeight));
}


void MainWindow::getBlendingResult(QImage& cImg, double dDensity)
{
    for (int x = 0; x < m_iWidth; x++)
    {
        for (int y = 0; y < m_iHeight; y++)
        {
            double size = dDensity;
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


    double dDensity = ui->densityLineEdit->text().toFloat();
    double dPro1 = ui->weightRed->text().toFloat();
    double dPro2 = ui->weightYellow->text().toFloat();
    double dPro3 = ui->weightBlue->text().toFloat();
    double dPro4 = ui->weightAdd1->text().toFloat();
    double dPro5 = ui->weightAdd2->text().toFloat();

    m_acAllWeights.clear();
    // first
    this->generateRandomNoise();
    this->generateSmoothNoise();
    this->getBlendingResult(cPerlinNoise, dDensity);
    this->displayResult(ui->random1, cPerlinNoise, cColor1);
    m_acAllWeights.push_back(cPerlinNoise);

    // second
    this->generateRandomNoise();
    this->generateSmoothNoise();
    this->getBlendingResult(cPerlinNoise, dDensity);
    this->displayResult(ui->random2, cPerlinNoise, cColor2);
    m_acAllWeights.push_back(cPerlinNoise);

    // third
    this->generateRandomNoise();
    this->generateSmoothNoise();
    this->getBlendingResult(cPerlinNoise, dDensity);
    this->displayResult(ui->random3, cPerlinNoise, cColor3);
    m_acAllWeights.push_back(cPerlinNoise);

    // forth
    this->generateRandomNoise();
    this->generateSmoothNoise();
    this->getBlendingResult(cPerlinNoise, dDensity);
    this->displayResult(ui->random4, cPerlinNoise, cColor4);
    m_acAllWeights.push_back(cPerlinNoise);

    // fifth
    this->generateRandomNoise();
    this->generateSmoothNoise();
    this->getBlendingResult(cPerlinNoise, dDensity);
    this->displayResult(ui->random5, cPerlinNoise, cColor5);
    m_acAllWeights.push_back(cPerlinNoise);

    QVector3D cBlending;
    for(int x = 0; x < m_iWidth; x++)
    {
        for(int y = 0; y < m_iHeight; y++)
        {
            double dWeight1 = qGray(m_acAllWeights[0].pixel(x,y))*dPro1/255.0;
            double dWeight2 = qGray(m_acAllWeights[1].pixel(x,y))*dPro2/255.0;
            double dWeight3 = qGray(m_acAllWeights[2].pixel(x,y))*dPro3/255.0;
            double dWeight4 = qGray(m_acAllWeights[3].pixel(x,y))*dPro4/255.0;
            double dWeight5 = qGray(m_acAllWeights[4].pixel(x,y))*dPro5/255.0;

            cBlending = cColor1*dWeight1 + cColor2*dWeight2 + cColor3*dWeight3 + cColor4*dWeight4 + cColor5*dWeight5;
            m_cResult.setPixel(x, y, ryb2rgb(cBlending.x(), cBlending.y(), cBlending.z()).rgb());
            //            int r = cColor1.red()*dWeight1 + cColor2.red()*dWeight2;
            //            cBlending.setRed(VALUE_CLIP(0, 255, r));
            //            int g = cColor1.green()*dWeight1 + cColor2.green()*dWeight2;
            //            cBlending.setGreen(VALUE_CLIP(0, 255, g));
            //            int b = cColor1.blue()*dWeight1 + cColor2.blue()*dWeight2;
            //            cBlending.setBlue(VALUE_CLIP(0, 255, b));


            //qDebug() << dWeight1 << dWeight2;
            //            m_cResult.setPixel(x, y, cBlending.rgba());
        }
    }
    m_cResult.save(QString("single_%1_%2_%3.png")
                   .arg(dPro1, 0, 'f', 1)
                   .arg(dPro2, 0, 'f', 1)
                   .arg(dPro3, 0, 'f', 1));


    this->displayResult(ui->resultLabel, m_cResult);

}

void MainWindow::on_readFileAndRunBtn_clicked()
{
    QString strFilepath = this->ui->filePathLineEdit->text();
    QFile cFile(strFilepath);
    if( !cFile.open(QIODevice::ReadOnly) )
    {
        qDebug() << QString("File Not Found: %1").arg(strFilepath);
        return;
    }


    QTextStream cInWeight(&cFile);
    double dDensity = ui->densityLineEdit->text().toFloat();

    QImage cPerlinNoise(m_iWidth, m_iHeight, QImage::Format_RGB32);
    int iLineNum = 0;
    while( !cInWeight.atEnd() )
    {
        iLineNum++;
        QStringList strLine = cInWeight.readLine().trimmed().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if(strLine.isEmpty())
        {
            qDebug() << "#" << iLineNum << "line is empty";
            continue;
        }
        //qDebug() << strLine;
        Q_ASSERT(strLine.size() == 3);
        double dPro1 = strLine[0].toFloat();
        double dPro2 = strLine[1].toFloat();
        double dPro3 = strLine[2].toFloat();
        //qDebug() << dPro1 << dPro2 << dPro3;

        m_acAllWeights.clear();
        // first
        this->generateRandomNoise();
        this->generateSmoothNoise();
        this->getBlendingResult(cPerlinNoise, dDensity);
        this->displayResult(ui->random1, cPerlinNoise);
        m_acAllWeights.push_back(cPerlinNoise);

        // second
        this->generateRandomNoise();
        this->generateSmoothNoise();
        this->getBlendingResult(cPerlinNoise, dDensity);
        this->displayResult(ui->random2, cPerlinNoise);
        m_acAllWeights.push_back(cPerlinNoise);

        // third
        this->generateRandomNoise();
        this->generateSmoothNoise();
        this->getBlendingResult(cPerlinNoise, dDensity);
        this->displayResult(ui->random3, cPerlinNoise);
        m_acAllWeights.push_back(cPerlinNoise);

        // result
        for(int x = 0; x < m_iWidth; x++)
        {
            for(int y = 0; y < m_iHeight; y++)
            {
                double dWeight1 = qGray(m_acAllWeights[0].pixel(x,y))*dPro1/255.0;
                double dWeight2 = qGray(m_acAllWeights[1].pixel(x,y))*dPro2/255.0;
                double dWeight3 = qGray(m_acAllWeights[2].pixel(x,y))*dPro3/255.0;
                m_cResult.setPixel(x, y, ryb2rgb(dWeight1, dWeight2, dWeight3).rgb());
            }
        }

//        m_cResult.save(QString("#%1_%2_%3_%4.png").arg(iLineNum)
//                       .arg(dPro1, 0, 'f', 1)
//                       .arg(dPro2, 0, 'f', 1)
//                       .arg(dPro3, 0, 'f', 1));
        m_cResult.save(QString("#%1.jpg").arg(iLineNum));


        qDebug() << "#" << iLineNum << "line finished";
    }
    //this->displayResult(ui->resultLabel, m_cResult);
}
