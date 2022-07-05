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
#include "lviFile.h"
#include "cplFile.h"

class RepositoryCommandThread : public Poco::Runnable
{

public:

    RepositoryCommandThread();
    ~RepositoryCommandThread();
    void startThread();
    void stopThread() {stop = true;}
    void addCommand(std::shared_ptr<Command> cmd);

protected:

    void run();
    void executeCommand(std::shared_ptr<Command> cmd);

private:

    Poco::Mutex commandsMutex;
    std::map<std::string, std::shared_ptr<Command>> commands;
    void synchronizeScriptById(std::string idSyncRef, std::string idCplNew, RepositoryResponse* response);
    void synchronizeScriptByDuration(std::string idSyncRef, std::string idCplNew, RepositoryResponse* response);

    // -- for thread control
	Poco::Thread* thread;
	bool stop;

};