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

class KinetDiscoveryThread : public Poco::Runnable
{

public:

    KinetDiscoveryThread();
    ~KinetDiscoveryThread();
    void startThread();
    void stopThread();

protected:

    void run();

private:

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};