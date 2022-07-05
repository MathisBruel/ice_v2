#include <cstdlib>
#include <iostream>
#include <queue>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "ApplicationContext.h"

class PlayoutThread : public Poco::Runnable
{

public:

    PlayoutThread(Device* device, std::shared_ptr<Image> img, bool lutApplied = true);
    ~PlayoutThread();

    bool isRunning() {return running;}

protected:

    void run();

private:

    bool running;

    // -- pointer to device to compute (if panel and to send out to kinet)
    Device* device;
    std::shared_ptr<Image> img;
    bool lutApplied;

};