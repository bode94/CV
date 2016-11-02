#include "MainWindow.hpp"
#include <QDebug>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    /* initialisiere die UI Komponeten */
    setupUi();
}

MainWindow::~MainWindow()
{
    /* loesche die UI Komponeten */
    delete centralWidget;    
    
    /* schliesse alle offenen Fenster */
    destroyAllWindows();
}

/* Methode oeffnet ein Bild und zeigt es in einem separaten Fenster an */
void MainWindow::on_pbOpenImage_clicked()
{
    /* oeffne Bild mit Hilfe eines Dateidialogs */
    QString imagePath = QFileDialog::getOpenFileName(this, "Open Image...", QString(), QString("Images *.png *.jpg *.tiff *.tif *.jpeg"));
    
    /* wenn ein gueltiger Dateipfad angegeben worden ist... */
    if(!imagePath.isNull() && !imagePath.isEmpty())
    {
        /* ...lese das Bild ein */
        Mat img = ImageReader::readImage(QtOpencvCore::qstr2str(imagePath));
        
        /* wenn das Bild erfolgreich eingelesen worden ist... */
        if(!img.empty())
        {
            /* ... merke das Originalbild ... */
            originalImage = img;
            
            /* ... aktiviere das UI ... */
            enableGUI();
            
            /* ... zeige das Originalbild in einem separaten Fenster an */
            imshow("Original Image", originalImage);
        }
        else
        {
            /* ...sonst deaktiviere das UI */
            disableGUI();
        }
    }
}

bool MainWindow::compareSeam(const MapSeam &m1, const MapSeam &m2)
{
    return m1.value < m2.value;
}

void MainWindow::on_pbComputeSeams_clicked()
{
    /* Anzahl der Spalten, die entfernt werden sollen */
    int colsToRemove = sbCols->value();
    
    /* Anzahl der Zeilen, die entfernt werden sollen */
    int rowsToRemove = sbRows->value();
    
    /* .............. */
    pixelBrightness = Mat(originalImage.rows, originalImage.cols, CV_8U);
    for (int i = 0; i < originalImage.rows; i++) {
        for (int j = 0; j < originalImage.cols; j++) {
            Vec3b bgr = originalImage.at<Vec3b>(Point(j, i));
            pixelBrightness.at<uchar>(Point(j, i)) = brightness(bgr);
        }
    }

    imshow("Brightness", pixelBrightness);
    partitalSeams = QVector<int>(originalImage.cols * originalImage.rows);
    partitalSeams.fill(-1);
    for (int i = 1; i < originalImage.rows; i++) {
        for (int j = 0; j < originalImage.cols; j++) {
            minEnergy(i, j);
            debug++;
            //qDebug() << debug;
        }
    }
    qDebug() << "Finished calculating Seams";
    QVector<MapSeam> seams = QVector<MapSeam>(0);
    for (int j = 1; j <= originalImage.cols; j++) {
        MapSeam m;
        m.value = partitalSeams.at(originalImage.rows * originalImage.cols - j);
        m.column = originalImage.cols - j;
        //qDebug() << m.column << m.value;
        //if (partitalSeams.at(originalImage.rows * originalImage.cols - j) == 0) qDebug() << j;
        seams.append(m);
    }
    qSort(seams);

    QVector<QVector<int>> seamsToRemove = QVector<QVector<int>>(colsToRemove);
    for (int i = 0; i < colsToRemove; i++) {
        seamsToRemove.append(backtrackSeam(seams.at(i).column));
    }
    //QVector<int> seamToRemove = backtrackSeam(seams.at(0).column);
    markedImage = QVector<bool>(originalImage.rows * originalImage.cols);
    markedImage.fill(false);
    for (int i = 0; i < seamsToRemove.size(); i++) {
        for(int j = 0; j < seamsToRemove.at(i).size(); j++) {
            markedImage.replace(index(originalImage.rows - j - 1, seamsToRemove.at(i).at(j), originalImage.cols), true);
        }
    }
    croppedImage = Mat(originalImage.rows, originalImage.cols - colsToRemove, originalImage.type());
    int colsRemoved = 0;
    for (int i = 0; i < originalImage.rows; i++) {
        for (int j = 0; j < originalImage.cols; j++) {
            if (markedImage.at(index(i, j, originalImage.cols))) {
                colsRemoved++;
                continue;
            } else {
                if (colsRemoved > 0) qDebug() << "worked";
                croppedImage.at<Vec3b>(i, j - colsRemoved) = originalImage.at<Vec3b>(i, j);
            }
        }
        colsRemoved = 0;
    }
    /*croppedImage = Mat(originalImage.rows, originalImage.cols - colsToRemove, Vec3b.type);
    for (int i = 0; i < columnsToRemove; i++) {
        originalImage.row(0).
    }*/
    qDebug() << originalImage.cols * originalImage.rows << "pixel finished";
}

void MainWindow::on_pbRemoveSeams_clicked()
{
    /* .............. */
    imshow("Cropped Image", croppedImage);
}

void MainWindow::setupUi()
{
    /* Boilerplate code */
    /*********************************************************************************************/
    resize(129, 211);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setSizePolicy(sizePolicy);
    setMinimumSize(QSize(129, 211));
    setMaximumSize(QSize(129, 211));
    centralWidget = new QWidget(this);
    centralWidget->setObjectName(QString("centralWidget"));
    
    horizontalLayout = new QHBoxLayout(centralWidget);
    verticalLayout = new QVBoxLayout();
    
    pbOpenImage = new QPushButton(QString("Open Image"), centralWidget);
    verticalLayout->addWidget(pbOpenImage);
    
    
    verticalLayout_3 = new QVBoxLayout();
    lCaption = new QLabel(QString("Remove"), centralWidget);
    lCaption->setEnabled(false);
    verticalLayout_3->addWidget(lCaption);
    
    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setObjectName(QString("horizontalLayout_3"));
    lCols = new QLabel(QString("Cols"), centralWidget);
    lCols->setEnabled(false);
    lRows = new QLabel(QString("Rows"), centralWidget);
    lRows->setEnabled(false);
    horizontalLayout_3->addWidget(lCols);
    horizontalLayout_3->addWidget(lRows);
    verticalLayout_3->addLayout(horizontalLayout_3);
    
    horizontalLayout_2 = new QHBoxLayout();
    sbCols = new QSpinBox(centralWidget);
    sbCols->setEnabled(false);
    horizontalLayout_2->addWidget(sbCols);
    sbRows = new QSpinBox(centralWidget);
    sbRows->setEnabled(false);
    horizontalLayout_2->addWidget(sbRows);
    verticalLayout_3->addLayout(horizontalLayout_2);
    verticalLayout->addLayout(verticalLayout_3);
    
    pbComputeSeams = new QPushButton(QString("Compute Seams"), centralWidget);
    pbComputeSeams->setEnabled(false);
    verticalLayout->addWidget(pbComputeSeams);
    
    pbRemoveSeams = new QPushButton(QString("Remove Seams"), centralWidget);
    pbRemoveSeams->setEnabled(false);
    verticalLayout->addWidget(pbRemoveSeams);
    
    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);
    horizontalLayout->addLayout(verticalLayout);
    
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);
    setCentralWidget(centralWidget);
    /*********************************************************************************************/
    
    
    /* Verbindung zwischen den Buttonklicks und den Methoden, die beim jeweiligen Buttonklick ausgefuehrt werden sollen */
    connect(pbOpenImage,    &QPushButton::clicked, this, &MainWindow::on_pbOpenImage_clicked);  
    connect(pbComputeSeams, &QPushButton::clicked, this, &MainWindow::on_pbComputeSeams_clicked); 
    connect(pbRemoveSeams,  &QPushButton::clicked, this, &MainWindow::on_pbRemoveSeams_clicked);
}

void MainWindow::enableGUI()
{
    lCaption->setEnabled(true);
    
    lCols->setEnabled(true);
    lRows->setEnabled(true);
    
    sbCols->setEnabled(true);
    sbRows->setEnabled(true);
    
    pbComputeSeams->setEnabled(true);
    pbRemoveSeams->setEnabled(true);
    
    sbRows->setMinimum(0);
    sbRows->setMaximum(originalImage.rows);
    sbRows->setValue(2);
    
    sbCols->setMinimum(0);
    sbCols->setMaximum(originalImage.cols);
    sbCols->setValue(2);
}

void MainWindow::disableGUI()
{
    lCaption->setEnabled(false);
    
    lCols->setEnabled(false);
    lRows->setEnabled(false);
    
    sbCols->setEnabled(false);
    sbRows->setEnabled(false);
    
    pbComputeSeams->setEnabled(false);
    pbRemoveSeams->setEnabled(false);
}

int MainWindow::energyFunction(const int x, const int y) {
    uchar b = pixelBrightness.at<uchar>(Point(y, x));
    uchar colBefore = y == 0 ? pixelBrightness.at<uchar>(Point(y - 1, x)) : pixelBrightness.at<uchar>(Point(y + 1, x));
    uchar rowBefore = pixelBrightness.at<uchar>(Point(y, x - 1));
    //qDebug() << qAbs(b - colBefore) + qAbs(b - rowBefore);
    return qAbs(b - colBefore) + qAbs(b - rowBefore);
}

uchar MainWindow::brightness(const Vec3b& bgr)
{
    return 0.299*bgr[2] + 0.587*bgr[1] + 0.114*bgr[0];
}

int MainWindow::index(const int i, const int j, const int width) {
    return i * width + j;
}

float MainWindow::minEnergy(int i, int j) {
    if (i == 0 || j > originalImage.cols || j < 0) {
        return 0;
    }
    int value = partitalSeams.at(index(i, j, originalImage.cols));
    if (value != -1) {
        //qDebug() << "found value: " << value;
        return value;
    }
    int m = energyFunction(i, j) + qMin(minEnergy(i-1, j-1), qMin(minEnergy(i-1, j), minEnergy(i-1, j+1)));
    //qDebug() << "partital Seam for (" << i << ", " << j << ") : " << m;
    //qDebug() << "energyFunction: " << energyFunction(i, j);
    partitalSeams.replace(index(i, j, originalImage.cols), m);
    return m;
}

QVector<int> MainWindow::backtrackSeam(int j) {
    QVector<int> seam = QVector<int>();
    for (int i = originalImage.rows - 1; i >= 0; i--) {
        float left = j > 0 ? partitalSeams.at(index(i, j - 1, originalImage.cols)) : INFINITY;
        float mid = partitalSeams.at(index(i, j, originalImage.cols));
        float right = j < originalImage.cols ? partitalSeams.at(index(i, j + 1, originalImage.cols)) : INFINITY;
        float min = qMin(left, qMin(mid, right));
        if (min == left) seam.append(--j);
        else if (min == mid) seam.append(j);
        else if (min == right) seam.append(j++);
    }
    return seam;
}
