#include <QMainWindow>
#include <QSlider>
#include <QWidget>
#include <QBoxLayout>
#include <QTimer>
#include <QLineEdit>
#include <QSpinBox>
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
    void zoomChanged();
    void ratioChanged();
    void pointerChanged();
    void minChanged();
    void maxChanged();

private:

    // -- gerneral
    QLabel* labelDevice;
    QLineEdit* device;
    QLabel* labelIp;
    QLineEdit* ipICE;
    QPushButton* connectToICE;
    QLabel* labelZoom;
    QSlider* zoom;
    
    // -- first config
    QLabel* labelPointer;
    QSlider* pointer;
    QLabel* labelPosition;
    QSpinBox* posX;
    QSpinBox* posY;
    QSpinBox* posZ;

    // -- seat zone 
    QLabel* labelMin;
    QSpinBox* minX;
    QSpinBox* minY;
    QSpinBox* minZ;
    QLabel* labelMax;
    QSpinBox* maxX;
    QSpinBox* maxY;
    QSpinBox* maxZ;

    Render* render;
    QTimer* timer;
};
