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
#include "ContextResponse.h"

class ContextThread : public Poco::Runnable
{

public:

    ContextThread();
    ~ContextThread();
    void startThread();
    void stopThread() {stop = true;}
    void addCommand(std::shared_ptr<Command> cmd);

protected:

    void run();
    void executeCommand(std::shared_ptr<Command> cmd);

private:

    std::map<std::string, std::shared_ptr<Command>> commands;
    Poco::Mutex commandsMutex;

    void handleStopScriptCmd(ContextResponse* response);
    void handleSwitchProjectionCmd(Command* cmd, ContextResponse* response);
    void handleSwitchLutCmd(Command* cmd, ContextResponse* response);
    void handleUnforceCmd(Command* cmd, ContextResponse* response);
    void handleUnforceAllCmd(ContextResponse* response);
    void handleSwitchModeCmd(Command* cmd, ContextResponse* response);
    void handleLaunchScriptCmd(Command* cmd, ContextResponse* response);


    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};