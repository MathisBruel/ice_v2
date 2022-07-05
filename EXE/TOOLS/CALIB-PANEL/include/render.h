#include <stdlib.h>
#include <iostream>
#include <queue>
#include <QWidget>
#include <QObject>

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "ImsCommunicator.h"
#include "device/DoremiHandler.h"
#include "portable_timer.h"

#include "lut.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"

class Render : public QObject
{
    Q_OBJECT

public:

    explicit Render(LUT* lut, QWidget* color);
    ~Render();
    void connectICE(std::string ip);
    void connectIMS(std::string ip, std::string user, std::string pass);
    void changeDevice(std::string device) {this->device = device;}

public slots:

    void run();
    void resetEmission();

private:

    void handlePlay(std::string title, int frame);
    void playColor(int red, int green, int blue);

    ImsCommunicator* ims;
    std::string lastPlayed;

    Poco::Net::HTTPClientSession* sessionICE;
    std::string sessionId;
    std::string device;

    LUT* lut;
    QWidget* color;
};