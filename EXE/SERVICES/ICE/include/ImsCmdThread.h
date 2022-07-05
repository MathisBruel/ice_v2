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

class ImsCmdThread : public Poco::Runnable
{

public:

    ImsCmdThread();
    ~ImsCmdThread();
    void startThread();
    void stopThread() {stop = true;}
    void addCommand(std::shared_ptr<Command> cmd);

    bool isThreadReady() {return readyToStart;}

protected:

    void run();
    void executeCommand(std::shared_ptr<Command> cmd);

private:

    void fillCplsInfo();

    std::map<std::string, std::shared_ptr<Command>> commands;
    Poco::Mutex commandsMutex;

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

    // -- first cpl must be filled once before ready
    bool readyToStart;
};