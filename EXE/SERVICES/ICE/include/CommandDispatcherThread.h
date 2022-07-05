#include <cstdlib>
#include <iostream>
#include <queue>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "ApplicationContext.h"
#include "command.h"
#include "ImsCmdThread.h"
#include "KinetThread.h"
#include "RepositoryCommandThread.h"
#include "ContextThread.h"
#include "RendererThread.h"

class CommandDispatcherThread : public Poco::Runnable
{

public:

    CommandDispatcherThread(ImsCmdThread* imsthread, KinetThread* kinetThread, RepositoryCommandThread* repoCmdThread, 
                            ContextThread* contextThread, RendererThread* renderThread);
    ~CommandDispatcherThread();
    void startThread();
    void stopThread() {stop = true;}

protected:

    void run();

private:

    // -- for dispatching commands
    ImsCmdThread* imsThread;
    KinetThread* kinetThread;
    RepositoryCommandThread* repoCmdThread;
    ContextThread* contextThread;
    RendererThread* renderThread;

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};