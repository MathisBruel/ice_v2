#include <QMainWindow>
#include <QSlider>
#include <QWidget>
#include <QBoxLayout>
#include <QTimer>
#include <QLineEdit>
#include <QLabel> 
#include <QPushButton> 

#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "render.h"

#pragma once

class CALIB : public QWidget
{
    Q_OBJECT

public:
    explicit CALIB(QWidget *parent = nullptr);
    ~CALIB();

private slots:

    void deviceChanged();
    void connectICE();


    void frequencyChanged();
    void redChanged();
    void greenChanged();
    void blueChanged();
    void amberChanged();
    void whiteChanged();
    void intensityChanged();
    void zoomChanged();
    void panChanged();
    void tiltChanged();

private:

    // -- global
    QLabel* labelDevice;
    QLineEdit* device;
    QLabel* labelIp;
    QLineEdit* ipICE;
    QPushButton* connectToICE;

    // -- color
    QLineEdit* redEdit;
    QSlider* redSlider;
    QLineEdit* greenEdit;
    QSlider* greenSlider;
    QLineEdit* blueEdit;
    QSlider* blueSlider;
    QLineEdit* amberEdit;
    QSlider* amberSlider;
    QLineEdit* whiteEdit;
    QSlider* whiteSlider;

    // -- other param
    QLineEdit* intensityEdit;
    QSlider* intensitySlider;
    QLineEdit* zoomEdit;
    QSlider* zoomSlider;
    QLineEdit* panEdit;
    QSlider* panSlider;
    QLineEdit* tiltEdit;
    QSlider* tiltSlider;
    QLineEdit* timeEdit;
    QSlider* timeSlider;

    Render* render;
    QTimer* timer;
};
