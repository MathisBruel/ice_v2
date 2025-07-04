#pragma once
#include <cstdlib>
#include <iostream>
#include <queue>
#include <regex>
#include <memory>

#include "unistd.h"

#include "Poco/Runnable.h"
#include "Poco/Stopwatch.h"
#include "CentralContext.h"
#include "commandCentral.h"
#include "commandCentralResponse.h"
//#include "Configurator.h"
#include "ContentOpsBoundary/BoundaryManager.h"
#include "ContentOpsBoundary/COB_Configurator.h"
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

    //std::unordered_map<int, Configurator*> _contentConfigurator;
    // -- for thread control
	Poco::Thread* thread;
	bool stop;
    MySQLDBConnection * _dbConnection;
    BoundaryManager& _boundaryManager;
    std::unique_ptr<COB_Configurator> _cobConfigurator;
};