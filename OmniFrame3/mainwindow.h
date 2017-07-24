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

protected:

public slots:

    void inhibit();
    void plusMoment();

    void updateCurrent();
    void updatePosition();
    void updateOrientation();

    void updateSensor();
    void average();
    void sensorError_button();
    //void updateDemo();
    //void startTrajectoryMode();
    void updateTool();

private:
    Ui::MainWindow *ui;

    QTimer updateTimer;
    Setup*   thisSetup;
    Field_and_Force_Demo* thisDemo;
    QTimer *goal_Timer;
    enum MODE{WEIGHT, TRAJECTORY, NOTSET} mode;

    void rotate();
};

#endif // MAINWINDOW_H
