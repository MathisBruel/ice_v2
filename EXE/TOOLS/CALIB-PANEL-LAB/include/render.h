#include <stdlib.h>
#include <iostream>
#include <queue>
#include <QWidget>
#include <QObject>

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "IMSConfiguration.h"
#include "device/Device.h"
#include "device/LuminousTextile.h"
#include "KinetConfiguration.h"
#include "ImsCommunicator.h"
#include "device/DoremiHandler.h"
#include "device/ChristieHandler.h"
#include "device/BarcoHandler.h"
#include "portable_timer.h"

#include "labCalib.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"

class Render : public QObject
{
    Q_OBJECT

public:

    explicit Render(LabCalib* lab);
    ~Render();
    void init();

public slots:

    void run();
    void resetEmission();

private:

    void handlePlay(std::string title, int frame);
    void playColor(int red, int green, int blue);

    KinetConfiguration* kinetConf;
    IMSConfiguration* imsConf;
    ImsCommunicator* ims;
    std::string lastPlayed;

    Poco::Net::HTTPClientSession* sessionICE;

    std::string sessionId;

    LabCalib* lab;
};