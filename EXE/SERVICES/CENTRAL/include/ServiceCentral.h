#include <cstdlib>
#include <iostream>

#pragma once

#include "Poco/Util/ServerApplication.h"
#include "Poco/Logger.h"
#include "Poco/Channel.h"
#include "Poco/FileChannel.h"
#include "Poco/AsyncChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"

#include "CentralContext.h"
#include "ContextCentralThread.h"
#include "ReleasePrepareThread.h"
#include "StateServerThread.h"
#include "SynchroThread.h"

class ServiceCentral : public Poco::Util::ServerApplication
{

public:
    ServiceCentral();
    ~ServiceCentral();

protected:

    void initialize(Application& self);
    void reinitialize(Application& self);
    void uninitialize();

    void initLogger();
    bool initContext();
    bool initThreads();

    int main(const std::vector<std::string> &args);

private:

    std::string pathLog;
    Poco::Message::Priority priority;

    // -- threads
    ContextCentralThread* contextThread;
    ReleasePrepareThread* releaseThread;
    StateServerThread* serversThread;
    StateServerThread* syncThread;
};

POCO_SERVER_MAIN(ServiceCentral);