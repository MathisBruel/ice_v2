#include <cstdlib>
#include <iostream>
#include <queue>
#include <fstream>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "ApplicationContext.h"
#include "command.h"
#include "commandResponse.h"
#include "PlayoutThread.h"
#include "Poco/ThreadPool.h"

class KinetThread : public Poco::Runnable
{

public:

    KinetThread();
    ~KinetThread();
    void startThread();
    void stopThread();
    void addCommand(std::shared_ptr<Command> cmd);

protected:

    void run();
    void executeCommand(std::shared_ptr<Command> cmd);

private:

    void clearPlayoutThreads();

    Poco::Mutex commandsMutex;
    std::map<std::string, std::shared_ptr<Command>> commands;

    std::vector<PlayoutThread*> threadsPlayout;
    Poco::ThreadPool* poolOfThread;

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};