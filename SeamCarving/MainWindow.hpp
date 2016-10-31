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
    cv::Mat originalImage;
    cv::Mat croppedImage;
    QVector<float> partitalSeams;
    struct MapSeam {
        float value;
        int column;
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
    float brightness(const cv::Vec3b &bgr);
    int index(const int i, const int j, const int width);
    float energyFunction(int x, int y);
    float minEnergy(int i, int j);
    bool compareSeam(const MapSeam &m1, const MapSeam &m2);
    QVector<int> backtrackSeam(int j);
};

#endif // MAINWINDOW_HPP
