#include <cstdlib>
#include <iostream>
#include <queue>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "ApplicationContext.h"
#include "command.h"
#include "RepositoryResponse.h"
#include "synchro.h"

class RepositoryThread : public Poco::Runnable
{

public:

    RepositoryThread();
    ~RepositoryThread();
    void startThread();
    void stopThread();

protected:

    void run();

private:

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};