#include <cstdlib>
#include <iostream>
#include <queue>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "CentralContext.h"
#include "commandCentral.h"
#include "commandCentralResponse.h"

class ContextCentralThread : public Poco::Runnable
{

public:

    ContextCentralThread();
    ~ContextCentralThread();
    void startThread();
    void stopThread() {stop = true;}

protected:

    void run();
    void executeCommand(std::shared_ptr<CommandCentral> cmd);

private:

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};