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
    void changePan(double pan) {this->pan = pan;}
    void changeTilt(double tilt) {this->tilt = tilt;}

public slots:

    void run();

private:

    std::string device;
    Poco::Net::HTTPClientSession* sessionICE;
    std::string sessionId;
    
    double pan;
    double tilt;
};