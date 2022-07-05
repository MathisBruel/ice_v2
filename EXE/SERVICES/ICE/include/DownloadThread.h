#include <cstdlib>
#include <iostream>
#include <queue>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "ApplicationContext.h"

class DownloadThread : public Poco::Runnable
{

public:

    DownloadThread();
    ~DownloadThread();

    void startThread();
    void stopThread() {stop = true;}

protected:

    void run();

private:

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};