#include <cstdlib>
#include <iostream>
#include <queue>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "ApplicationContext.h"
#include "command.h"
#include "commandResponse.h"
//#include "ImsCmdThread.h"

class ImsThread : public Poco::Runnable
{

public:

    ImsThread();
    ~ImsThread();
    void startThread();
    void stopThread() {stop = true;}

protected:

    void run();

private:

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};