#include <stdlib.h>
#include <iostream>
#include <queue>
#include <QWidget>
#include <QObject>

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "portable_timer.h"

#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPRequest.h"

class Render : public QObject
{
    Q_OBJECT

public:

    explicit Render();
    ~Render();

    void connectICE(std::string ip);
    void changeDevice(std::string device) {this->device = device;}
    void changeRed(int red) {this->red = red;}
    void changeGreen(int green) {this->green = green;}
    void changeBlue(int blue) {this->blue = blue;}
    void changeAmber(int amber) {this->amber = amber;}
    void changeWhite(int white) {this->white = white;}
    void changeIntensity(int intensity) {this->intensity = intensity;}
    void changeZoom(int zoom) {this->zoom = zoom;}
    void changePan(int pan) {this->pan = pan;}
    void changeTilt(int tilt) {this->tilt = tilt;}

public slots:

    void run();

private:

    int red;
    int green;
    int blue;
    int amber;
    int white;
    int intensity;
    int zoom;
    int pan, tilt;
    std::string device;
    std::string ip;

    Poco::Net::HTTPClientSession* sessionICE;
    std::string sessionId;

    bool rgb;
};