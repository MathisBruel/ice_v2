#include <cstdlib>
#include <iostream>
#include <queue>
#include <regex>

#include "unistd.h"

#pragma once
#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "CentralContext.h"
#include "commandCentral.h"
#include "commandCentralResponse.h"
#include "Configurator.h"
#include "Infra/MySQLDBConnection.h"
#include "App/StateMachineManager.h"

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

    std::unordered_map<int, Configurator*> _contentConfigurator;
    // -- for thread control
	Poco::Thread* thread;
	bool stop;
    MySQLDBConnection * _dbConnection;
};