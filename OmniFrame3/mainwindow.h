#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "Setup.h"

#include <QString>
#include <QTimer>


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

protected:

public slots:

    void inhibit();
    void plusMoment();

    void updateCurrent();
    void updateOrientation();

private:
    Ui::MainWindow *ui;

    QTimer updateTimer;
    Setup*   thisSetup;
};

#endif // MAINWINDOW_H
