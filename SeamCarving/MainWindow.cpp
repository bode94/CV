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

void MainWindow::on_pbComputeSeams_clicked()
{
    if (debug == 0) seamImage = originalImage.clone();
    debug++;
    if (sbCols->value() == 0 && sbRows->value() == 0) return;
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
    
    if (colsToRemove > 0) removeVerticalSeam();
    if (rowsToRemove > 0) removeHorizontalSeam();

    sbCols->setValue(colsToRemove - 1);
    sbRows->setValue(rowsToRemove - 1);
    imshow("Seam", seamImage);
    on_pbComputeSeams_clicked();
}

void MainWindow::removeVerticalSeam() {
    vPartitalSeams = QVector<int>(originalImage.cols * originalImage.rows);
    vPartitalSeams.fill(-1);
    for (int i = 1; i < originalImage.rows; i++) {
        for (int j = 0; j < originalImage.cols; j++) {
            vMinEnergy(i, j);
        }
    }

    QVector<MapSeam> vSeams = QVector<MapSeam>();
    for (int j = 1; j <= originalImage.cols; j++) {
        MapSeam m;
        m.value = vPartitalSeams.at(originalImage.rows * originalImage.cols - j);
        m.pos = originalImage.cols - j;
        vSeams.append(m);
    }
    MapSeam vmin = *std::min_element(vSeams.begin(), vSeams.end());
    QVector<int> vSeamToRemove = vBacktrackSeam(vmin.pos);

    vMarkedImage = QVector<bool>(originalImage.rows * originalImage.cols);
    vMarkedImage.fill(false);
    for (int i = 0; i < vSeamToRemove.size(); i++) {
        vMarkedImage.replace(index(originalImage.rows - i - 1, vSeamToRemove[i], originalImage.cols), true);
        seamImage.at<Vec3b>(originalImage.rows - i - 1, vSeamToRemove[i]) = Vec3b(0,0,255);
    }

    croppedImage = Mat(originalImage.rows, originalImage.cols - 1, originalImage.type());
    int colsRemoved = 0;
    for (int i = 0; i < originalImage.rows; i++) {
        for (int j = 0; j < originalImage.cols; j++) {
            if (vMarkedImage.at(index(i, j, originalImage.cols))) {
                colsRemoved++;
                continue;
            } else {
                //if (colsRemoved > 0) qDebug() << "worked";
                croppedImage.at<Vec3b>(i, j - colsRemoved) = originalImage.at<Vec3b>(i, j);
            }
        }
        colsRemoved = 0;
    }
    originalImage = croppedImage.clone();
}

void MainWindow::removeHorizontalSeam() {
    hPartitalSeams = QVector<int>(originalImage.cols * originalImage.rows);
    hPartitalSeams.fill(-1);
    for (int i = 1; i < originalImage.cols; i++) {
        for (int j = 0; j < originalImage.rows; j++) {
            hMinEnergy(i, j);
        }
    }
    QVector<MapSeam> hSeams = QVector<MapSeam>();
    for (int j = 1; j <= originalImage.rows; j++) {
        MapSeam m;
        m.value = hPartitalSeams.at(originalImage.rows * originalImage.cols - j);
        m.pos = originalImage.rows - j;
        hSeams.append(m);
    }
    MapSeam hmin = *std::min_element(hSeams.begin(), hSeams.end());
    QVector<int> hSeamToRemove = hBacktrackSeam(hmin.pos);
    hMarkedImage = QVector<bool>(originalImage.rows * originalImage.cols);
    hMarkedImage.fill(false);
    for (int i = 0; i < hSeamToRemove.size(); i++) {
        hMarkedImage.replace(index(hSeamToRemove[i], originalImage.cols - i - 1, originalImage.cols), true);
        seamImage.at<Vec3b>(hSeamToRemove[i], originalImage.cols - i - 1) = Vec3b(0,0,255);
    }
    //qDebug() << hMarkedImage;
    croppedImage = Mat(originalImage.rows - 1, originalImage.cols, originalImage.type());
    int rowsRemoved = 0;
    for (int col = 0; col < originalImage.cols; col++) {
        for (int row = 0; row < originalImage.rows; row++) {
            //croppedImage.at<Vec3b>(0, 10) = originalImage.at<Vec3b>(1, 10);
            if (hMarkedImage.at(index(row, col, originalImage.cols))) {
                rowsRemoved++;
                continue;
            } else {
                croppedImage.at<Vec3b>(row - rowsRemoved, col) = originalImage.at<Vec3b>(row, col);
            }
        }
        rowsRemoved = 0;
    }
    originalImage = croppedImage.clone();
}

void MainWindow::on_pbRemoveSeams_clicked()
{
    /* .............. */
    imshow("Cropped Image", croppedImage);
}

uchar MainWindow::brightness(const Vec3b& bgr)
{
    return 0.299*bgr[2] + 0.587*bgr[1] + 0.114*bgr[0];
}

int MainWindow::index(const int i, const int j, const int width) {
    return i * width + j;
}

int MainWindow::energyFunction(const int x, const int y) {
    uchar b = pixelBrightness.at<uchar>(Point(y, x));
    uchar colBefore = y > 0 ? pixelBrightness.at<uchar>(Point(y - 1, x)) : INFINITY;
    uchar rowBefore = x > 0 ? pixelBrightness.at<uchar>(Point(y, x - 1)) : INFINITY;
    //qDebug() << qAbs(b - colBefore) + qAbs(b - rowBefore);
    return qAbs(b - colBefore) + qAbs(b - rowBefore);
}

float MainWindow::vMinEnergy(int row, int col) {
    if (col < 0 || col >= originalImage.cols) return INFINITY;
    if (row == 0) return energyFunction(row, col);
    int value = vPartitalSeams.at(index(row, col, originalImage.cols));
    if (value != -1) return value;
    else {
        int m = energyFunction(row, col) + qMin(vMinEnergy(row-1, col-1), qMin(vMinEnergy(row-1, col), vMinEnergy(row-1, col+1)));
        vPartitalSeams.replace(index(row, col, originalImage.cols), m);
        return m;
    }
}

float MainWindow::hMinEnergy(int col, int row) {
    if (row >= originalImage.rows || row < 0) return INFINITY;
    if (col == 0) return energyFunction(row, col);
    int value = hPartitalSeams.at(index(col, row, originalImage.rows));
    if (value != -1) return value;
    else {
        int m = energyFunction(row, col) + qMin(hMinEnergy(col-1, row-1), qMin(hMinEnergy(col-1, row), hMinEnergy(col-1, row+1)));
        hPartitalSeams.replace(index(col, row, originalImage.rows), m);
        return m;
    }
}

QVector<int> MainWindow::vBacktrackSeam(int col) {
    QVector<int> seam = QVector<int>();
    seam.append(col);
    for (int i = originalImage.rows - 1; i > 0; i--) {
        float left = col > 0 ? vPartitalSeams.at(index(i, col - 1, originalImage.cols)) : INFINITY;
        float mid = vPartitalSeams.at(index(i, col, originalImage.cols));
        float right = col < originalImage.cols ? vPartitalSeams.at(index(i, col + 1, originalImage.cols)) : INFINITY;
        float min = qMin(left, qMin(mid, right));
        if (min == left) seam.append(--col);
        else if (min == mid) seam.append(col);
        else if (min == right) seam.append(col++);
    }
    return seam;
}

QVector<int> MainWindow::hBacktrackSeam(int row) {
    QVector<int> seam = QVector<int>();
    seam.append(row);
    for (int i = originalImage.cols - 1; i > 0; i--) {
        float below = row > 0 ? hPartitalSeams.at(index(i, row - 1, originalImage.rows)) : INFINITY;
        float mid = hPartitalSeams.at(index(i, row, originalImage.rows));
        float above = row < originalImage.rows ? hPartitalSeams.at(index(i, row + 1, originalImage.rows)) : INFINITY;
        float min = qMin(below, qMin(mid, above));
        if (min == below) seam.append(--row);
        else if (min == mid) seam.append(row);
        else if (min == above) seam.append(row++);
    }
    return seam;
}
