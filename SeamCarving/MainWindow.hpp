#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QFileDialog>
#include <QPushButton>
#include <QBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QStatusBar>
#include <QtCore/qmath.h>
#include <QtGlobal>

#include "ImageReader.hpp"
#include "QtOpencvCore.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    
    /* Konstruktor */
    explicit MainWindow(QWidget *parent = 0);
    
    /* Destruktor */
    ~MainWindow();
    
private slots:  
    
    /* Funktionen werden ausgelöst, wenn auf den entsprechenden Button geklickt wird */
    void on_pbOpenImage_clicked();
    void on_pbComputeSeams_clicked();
    void on_pbRemoveSeams_clicked();
    
private:

    
    /* GUI Komoneten */
    QWidget     *centralWidget;
    
    QHBoxLayout *horizontalLayout;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_3;
    
    QPushButton *pbOpenImage;
    QPushButton *pbRemoveSeams;
    QPushButton *pbComputeSeams;
    
    QLabel      *lCaption;
    QLabel      *lCols;
    QLabel      *lRows;
    
    QSpinBox    *sbCols;
    QSpinBox    *sbRows;
    
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    /*****************************************/
    
    /* Originalbild */
    int debug = 0;
    cv::Mat originalImage;
    cv::Mat pixelBrightness;
    cv::Mat seamImage;
    QVector<bool> vMarkedImage;
    QVector<bool> hMarkedImage;
    cv::Mat croppedImage;
    QVector<int> vPartitalSeams;
    QVector<int> hPartitalSeams;
    struct MapSeam {
        float value;
        int pos;
        bool operator<(const MapSeam& m) const{
            return value < m.value;
        }
    };
    /* Eventuel weitere Klassenattribute */
    
    /* Methode initialisiert die UI */
    void setupUi();
    
    /* Methoden aktivieren bzw. deaktivieren die UI */
    void enableGUI();
    void disableGUI();
    void removeVerticalSeam();
    void removeHorizontalSeam();
    uchar brightness(const cv::Vec3b &bgr);
    int index(const int i, const int j, const int width);
    int energyFunction(const int x, const int y);
    float vMinEnergy(int row, int col);
    float hMinEnergy(int col, int row);
    QVector<int> vBacktrackSeam(int col);
    QVector<int> hBacktrackSeam(int row);
};

#endif // MAINWINDOW_HPP
