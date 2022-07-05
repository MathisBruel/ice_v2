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

#define M_PI 3.14159265358979323846

class Render : public QObject
{
    Q_OBJECT

public:

    explicit Render();
    ~Render();

    void connectICE(std::string ip);
    void changeDevice(std::string device);
    void changeZoom(int zoom) {this->zoom = zoom;}
    void changeRatio(int ratio) {this->ratio = ratio;}
    void changePointer(int x, int y, int z) {posX = x; posY = y; posZ = z;}
    void changeMin(int x, int y, int z) {minX = x; minY = y; minZ = z;}
    void changeMax(int x, int y, int z) {maxX = x; maxY = y; maxZ = z;}

public slots:
 
    void run();

private:

    std::string device;
    Poco::Net::HTTPClientSession* sessionICE;
    std::string sessionId;

    int zoom;
    int ratio;
    bool isLeft;

    int posX, posY, posZ;
    int minX, minY, minZ;
    int maxX, maxY, maxZ;
};