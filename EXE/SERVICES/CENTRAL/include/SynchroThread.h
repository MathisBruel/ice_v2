#include <cstdlib>
#include <iostream>
#include <queue>
#include <map>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "CentralContext.h"

// -- ICE session
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"

#include "Data/Release.h"
#include "lviFile.h"
#include "cplFile.h"
#include "synchro.h"


class SynchroThread : public Poco::Runnable
{

public:

    SynchroThread();
    ~SynchroThread();
    void startThread();
    void stopThread() {stop = true;}

protected:

    void run();

private:

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};