#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "Setup.h"

#include <QString>
#include <QTimer>

#include "Demos/Field_and_Force_Demo.h"
#include "Producers/SpinVector.h"

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

    double minReading[4];
    double maxReading[4];

    SpinningVector spin;

    double* getFieldLocation();

protected:

public slots:

    //functions
    void inhibit();
    void rotatingField();
    void rotate();
    void plusMoment();

    //Lable update
    void updateCurrent();
    void updatePosition();
    void updateOrientation();
    void updateSensor();

    //Temp/development
    void average();
    void sensorError_button();
    void updateDemo();
    //void startTrajectoryMode();
    void updateToolTrajectory();
    void updateToolProporties();
    void omniSysCheck();
    void updateField();

private:
    Ui::MainWindow *ui;

    QTimer updateTimer;
    QTimer updateRotation;
    Setup*   thisSetup;
    Field_and_Force_Demo* thisDemo;
    QTimer *goal_Timer;
    enum MODE{WEIGHT, TRAJECTORY, NOTSET} mode;

};

#endif // MAINWINDOW_H
