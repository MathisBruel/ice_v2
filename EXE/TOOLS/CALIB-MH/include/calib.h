#include <QMainWindow>
#include <QSlider>
#include <QWidget>
#include <QBoxLayout>
#include <QTimer>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/Thread.h"
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
    void panChanged();
    void tiltChanged();

private:

    QLabel* labelDevice;
    QLineEdit* device;
    QLabel* labelIp;
    QLineEdit* ipICE;
    QPushButton* connectToICE;

    QSlider* pan;
    QLineEdit* panEdit;
    QSlider* tilt;
    QLineEdit* tiltEdit;

    Render* render;
    QTimer* timer;
};
