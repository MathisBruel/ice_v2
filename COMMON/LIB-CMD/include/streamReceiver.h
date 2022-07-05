#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <queue>

#pragma once

#include "Poco/Logger.h"
#include "Poco/Stopwatch.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Runnable.h"
#include "ByteArray.h"
#include "image.h"
#include "portable_timer.h"

class StreamReceiver : Poco::Runnable
{

public:

    StreamReceiver(std::string address, uint16_t port);
    ~StreamReceiver();

    bool startThread();
    void stopThread();

    uint32_t getVersion() {return version;}
    std::shared_ptr<Image> getLastAddedImage();
    void removeLastAddedImage();

    int getCurrentFrame();
    bool isLutApplied() {return lutApplied;}

protected:

    void run();

private:

    std::string address;
    uint16_t port;

    uint32_t version;
    uint32_t format;
    bool lutApplied;

    Poco::Net::ServerSocket* server;
    std::queue<std::shared_ptr<Image>> images;
    Poco::Mutex mutexImages;

    int currentFrame;

    Poco::Thread* thread;
	bool stop;
};