#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QWidget>
#include <QBoxLayout>
#include <QTimer>
#include <QColor>
#include <QFileDialog>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>

#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/Channel.h"
#include "Poco/FileChannel.h"
#include "Poco/AsyncChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/ConsoleChannel.h"
#include "render.h"
#include "graph.h"
#include "lut.h"

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
    void connectIMS();

    void loadLUT();
    void saveLUT();
    void redrawPoly(int index, double oldValue, double newValue);

private:

    // -- ICE
    QLabel* labelDevice;
    QLineEdit* device;
    QLabel* labelIp;
    QLineEdit* ipICE;
    QPushButton* connectToICE;

    // -- IMS
    QLabel* labelIpIms;
    QLineEdit* ipIMS;
    QLabel* labelUser;
    QLineEdit* user;
    QLabel* labelPass;
    QLineEdit* pass;
    QPushButton* connectToIMS;

    // -- render
    QPushButton* load;
    QPushButton* save;
    QWidget* renderColor;
    QCheckBox* mixerMode;

    Render* render;
    QTimer* timer;
    LUT* lut;

    QTabWidget* tab;
    Graph* redGraph;
    Graph* greenGraph;
    Graph* blueGraph;
    Graph* redOnGreenGraph;
    Graph* blueOnGreenGraph;
    Graph* redOnBlueGraph;
    Graph* greenOnBlueGraph;
    Graph* greenOnRedGraph;
    Graph* blueOnRedGraph;
};
